#include "keyctrl.h"
#include "pic.h"

// clang-format off
unsigned char normal_tab[] =
{
   0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
   0, 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
   0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
   'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
   '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
   0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
};

unsigned char shift_tab[] =
{
   0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
   0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
   0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
   'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
   0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0
};

unsigned char alt_tab[] =
{
   0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
   0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
};
// clang-format on

unsigned char asc_num_tab[] = {'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','};
unsigned char scan_num_tab[] = {8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51};

unsigned char code;
unsigned char prefix;
Key gather = {0, 0, 0};
char leds;

// status register bits
enum
{
    outpb = 0x01,
    inpb = 0x02,
    auxb = 0x20
};

// keyboard commands
enum
{
    cmd_set_led = 0xed,
    set_speed = 0xf3,
    cpu_reset = 0xfe,
};

// keyboard replies
typedef enum
{
    ack = 0xfa,
    resend = 0xfe,
} kbd_reply;

// keyboard code constants
enum
{
    break_bit = 0x80,
    prefix1 = 0xe0,
    prefix2 = 0xe1
};

bool key_decoded()
{
    bool done = false;

    // Die Tasten, die bei der MF II Tastatur gegenueber der aelteren
    // AT Tastatur hinzugekommen sind, senden immer erst eines von zwei
    // moeglichen Prefix Bytes.
    if (code == prefix1 || code == prefix2)
    {
        prefix = code;
        return false;
    }

    // Das Loslassen einer Taste ist eigentlich nur bei den "Modifier" Tasten
    // SHIFT, CTRL und ALT von Interesse, bei den anderen kann der Break Code
    // ignoriert werden.
    if (code & break_bit)
    {
        code &= ~break_bit; // Der Break Code einer Taste ist gleich dem
                            // Make Code mit gesetzten break_bit.
        switch (code)
        {
        case 42:
        case 54: gather.shift = false; break;
        case 56:
            if (prefix == prefix1)
                gather.alt_right = false;
            else
                gather.alt_left = false;
            break;
        case 29:
            if (prefix == prefix1)
                gather.ctrl_right = false;
            else
                gather.ctrl_left = false;
            break;
        }

        // Ein Prefix gilt immer nur fuer den unmittelbar nachfolgenden Code.
        // Also ist es jetzt abgehandelt.
        prefix = 0;

        // Mit einem Break-Code kann man nichts anfangen, also false liefern.
        return false;
    }

    // Eine Taste wurde gedrueckt. Bei den Modifier Tasten wie SHIFT, ALT,
    // NUM_LOCK etc. wird nur der interne Zustand geaendert. Durch den
    // Rueckgabewert 'false' wird angezeigt, dass die Tastatureingabe noch
    // nicht abgeschlossen ist. Bei den anderen Tasten werden ASCII
    // und Scancode eingetragen und ein 'true' fuer eine erfolgreiche
    // Tastaturabfrage zurueckgegeben, obwohl genaugenommen noch der Break-
    // code der Taste fehlt.

    switch (code)
    {
    case 42:
    case 54: gather.shift = true; break;
    case 56:
        if (prefix == prefix1)
            gather.alt_right = true;
        else
            gather.alt_left = true;
        break;
    case 29:
        if (prefix == prefix1)
            gather.ctrl_right = true;
        else
            gather.ctrl_left = true;
        break;
    case 58:
        gather.caps_lock = !gather.caps_lock;
        keyctrl_set_led(caps_lock_led, gather.caps_lock);
        break;
    case 70:
        gather.scroll_lock = !gather.scroll_lock;
        keyctrl_set_led(scroll_lock_led, gather.scroll_lock);
        break;
    case 69:                    // Numlock oder Pause ?
        if (gather.ctrl_left) // Pause Taste
        {
            // Auf alten Tastaturen konnte die Pause-Funktion wohl nur
            // ueber Ctrl+NumLock erreicht werden. Moderne MF-II Tastaturen
            // senden daher diese Codekombination, wenn Pause gemeint ist.
            // Die Pause Taste liefert zwar normalerweise keinen ASCII
            // Code, aber Nachgucken schadet auch nicht. In jedem Fall ist
            // die Taste nun komplett.
            get_ascii_code();
            done = true;
        }
        else // NumLock
        {
            gather.num_lock = !gather.num_lock;
            keyctrl_set_led(num_lock_led, gather.num_lock);
        }
        break;

    default: // alle anderen Tasten
        // ASCII Codes aus den entsprechenden Tabellen auslesen, fertig.
        get_ascii_code();
        done = true;
    }

    // Ein Prefix gilt immer nur fuer den unmittelbar nachfolgenden Code.
    // Also ist es jetzt abgehandelt.
    prefix = 0;

    if (done) // Tastaturabfrage abgeschlossen
        return true;
    else
        return false;
}

void get_ascii_code()
{
    // Sonderfall Scancode 53: Dieser Code wird sowohl von der Minustaste
    // des normalen Tastaturbereichs, als auch von der Divisionstaste des
    // Ziffernblocks gesendet. Damit in beiden Faellen ein Code heraus-
    // kommt, der der Aufschrift entspricht, muss im Falle des Ziffern-
    // blocks eine Umsetzung auf den richtigen Code der Divisionstaste
    // erfolgen.
    if (code == 53 && prefix == prefix1) // Divisionstaste des Ziffernblocks
    {
        gather.ascii = '/';
        gather.scancode = key_div;
    }

    // Anhand der Modifierbits muss die richtige Tabelle ausgewaehlt
    // werden. Der Einfachheit halber hat NumLock Vorrang vor Alt,
    // Shift und CapsLock. Fuer Ctrl gibt es keine eigene Tabelle.

    else if (gather.num_lock && !prefix && code >= 71 && code <= 83)
    {
        // Bei eingeschaltetem NumLock und der Betaetigung einer der
        // Tasten des separaten Ziffernblocks (Codes 71-83), sollen
        // nicht die Scancodes der Cursortasten, sondern ASCII und
        // Scancodes der ensprechenden Zifferntasten geliefert werden.
        // Die Tasten des Cursorblocks (prefix == prefix1) sollen
        // natuerlich weiterhin zur Cursorsteuerung genutzt werden
        // koennen. Sie senden dann uebrigens noch ein Shift, aber das
        // sollte nicht weiter stoeren.
        gather.ascii = asc_num_tab[code - 71];
        gather.scancode = scan_num_tab[code - 71];
    }
    else if (gather.alt_right)
    {
        gather.ascii = alt_tab[code];
        gather.scancode = code;
    }
    else if (gather.shift)
    {
        gather.ascii = shift_tab[code];
        gather.scancode = code;
    }
    else if (gather.caps_lock)
    { // Die Umschaltung soll nur bei Buchstaben gelten
        if ((code >= 16 && code <= 26) || (code >= 30 && code <= 40) || (code >= 44 && code <= 50))
        {
            gather.ascii = shift_tab[code];
            gather.scancode = code;
        }
        else
        {
            gather.ascii = normal_tab[code];
            gather.scancode = code;
        }
    }
    else
    {
        gather.ascii = normal_tab[code];
        gather.scancode = code;
    }
}

void keyctrl_init()
{
    // alle LEDs ausschalten (bei vielen PCs ist NumLock nach dem Booten an)
    keyctrl_set_led(caps_lock_led, false);
    keyctrl_set_led(scroll_lock_led, false);
    keyctrl_set_led(num_lock_led, false);

    // maximale Geschwindigkeit, minimale Verzoegerung
    keyctrl_set_repeat_rate(0, 0);
}

Key keyctrl_key_hit()
{
    Key invalid = {0, 0, 0};
    if (inb(0x64) & auxb)
        return invalid;

    code = inb(0x60);

    if (key_decoded())
        return gather;

    return invalid;
}

void keyctrl_reboot()
{
    int status;

    // Dem BIOS mitteilen, dass das Reset beabsichtigt war
    // und kein Speichertest durchgefuehrt werden muss.
    *(unsigned short *) 0x472 = 0x1234;

    // Der Tastaturcontroller soll das Reset ausloesen.
    do
    {
        status = inb(0x64);   // warten, bis das letzte Kommando
    } while ((status & inpb) != 0); // verarbeitet wurde.
    outb(0x64, cpu_reset);      // Reset
}

void keyctrl_set_repeat_rate(int speed, int delay)
{
    //scoped_interrupt_guard guard(PIC::keyboard);

    unsigned char data = (delay << 5 | speed) & 0b01111111;
    kbd_reply reply = resend;

    for (;;)
    {
        if (reply == resend)
            outb(0x60, set_speed);

        // Auf antwort von der Tastatur warten.
        while (inb(0x64) & inpb)
            ;
        reply = inb(0x60);
        if (reply == ack)
            break;
    }

    outb(0x60, data);
    while (inb(0x64) & inpb)
        ;
}

void keyctrl_set_led(key_led led, bool on)
{
    //scoped_interrupt_guard guard(PIC::keyboard);

    if (on)
        leds |= led;
    else
        leds &= ~led;
    leds &= 0b0111;

    kbd_reply reply = resend;
    for (;;)
    {
        if (reply == resend)
            outb(0x60, cmd_set_led);

        while (inb(0x64) & inpb)
            ;
        reply = inb(0x60);
        if (reply == ack)
            break;
    }

    outb(0x60, leds);
    while (inb(0x64) & inpb)
        ;
}
