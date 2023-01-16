#pragma once

void watch_plugin(void (*epilogue)());
void watch_set(unsigned int us, unsigned int iterations);
void watch_epilogue();
