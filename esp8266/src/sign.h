#pragma once

// functions for controlling the ^H sign

void sign_slack_command(const char* command);
bool sign_control(uint8_t red, uint8_t green, uint8_t blue);
void sign_debug();
