#include <iostream>

#define RES     "\033[0m"      // Reset

#define GREY    "\033[0;90m"   // Normal grey
#define RED     "\033[0;91m"   // Normal red
#define GREEN   "\033[0;92m"   // Normal green
#define YELLOW  "\033[0;93m"   // Normal yellow/orange
#define BLUE    "\033[0;96m"   // Normal cyan (blue-ish)

/* Other */
#define C_GREY      "\033[3;90m"   // Cursive grey
#define F_WHITE      "\033[2;97m"   // Faint white

/* Reversed background + bold text */
#define REV_BLCK "\u001b[40;1m" // Black background, bold text
#define REV_RED  "\u001b[41;1m" // Red background, bold text
#define REV_WHITE "\u001b[47;1;2m" // White background, bold+faint text
#define REV_BLUE "\u001b[44;1m" // Blue background, bold text
#define REV_GREEN   "\u001b[42;1m"
#define REV_CYAN   "\u001b[46;1m"

/* Normal colors */
#define BLACK   "\033[0;30m"   // Normal black
#define ORANGE  "\033[0;93m"   // Alias for YELLOW (orange-ish)

/* Bright colors */
#define BRIGHT_BLACK "\033[0;90m" // Bright black (same as GREY)
#define BRIGHT_RED   "\033[0;91m" // Bright red (same as RED)
#define BRIGHT_GREEN "\033[0;92m" // Bright green (same as GREEN)
#define BRIGHT_BLUE  "\033[0;94m" // Bright blue
#define BRIGHT_YELLOW "\033[0;93m" // Bright yellow/orange (same as YELLOW)


/* Bold bright colors */
#define BOLD_BRIGHT_BLACK "\033[1;90m" // Bold bright black (same as GB)
#define BOLD_BRIGHT_RED   "\033[1;91m" // Bold bright red
#define BOLD_BRIGHT_GREEN "\033[1;92m" // Bold bright green
#define BOLD_BRIGHT_BLUE  "\033[1;94m" // Bold bright blue
#define BOLD_BRIGHT_YELLOW "\033[1;93m" // Bold bright yellow/orange
#define BOLD_BRIGHT_GREY  "\033[1;37m" // Bold bright grey

int main() {

    std::cout << "Normal Colors:" << std::endl;
    std::cout << BLACK   << "[HELLO]" << RES << std::endl;
    std::cout << RED     << "[HELLO]" << RES << std::endl;
    std::cout << GREEN   << "[HELLO]" << RES << std::endl;
    std::cout << BLUE    << "[HELLO]" << RES << std::endl;
    std::cout << ORANGE  << "[HELLO]" << RES << std::endl;
    std::cout << GREY    << "[HELLO]" << RES << std::endl;
    std::cout << F_WHITE << "[HELLO]" << RES << std::endl; // Added F_WHITE (faint white)

    std::cout << "\nBright Colors:" << std::endl;
    std::cout << BRIGHT_BLACK  << "[HELLO]" << RES << std::endl;
    std::cout << BRIGHT_RED    << "[HELLO]" << RES << std::endl;
    std::cout << BRIGHT_GREEN  << "[HELLO]" << RES << std::endl;
    std::cout << BRIGHT_BLUE   << "[HELLO]" << RES << std::endl;
    std::cout << BRIGHT_YELLOW << "[HELLO]" << RES << std::endl;

    std::cout << "\nCursive Colors:" << std::endl;
    std::cout << C_GREY            << "[HELLO]" << RES << std::endl; // Added C_GREY (italic grey)

    // Bold bright colors
    std::cout << "\nBold Bright Colors:" << std::endl;
    std::cout << BOLD_BRIGHT_BLACK  << "[HELLO]" << RES << std::endl; // Same as GB
    std::cout << BOLD_BRIGHT_RED    << "[HELLO]" << RES << std::endl;
    std::cout << BOLD_BRIGHT_GREEN  << "[HELLO]" << RES << std::endl;
    std::cout << BOLD_BRIGHT_BLUE   << "[HELLO]" << RES << std::endl;
    std::cout << BOLD_BRIGHT_YELLOW << "[HELLO]" << RES << std::endl;
    std::cout << BOLD_BRIGHT_GREY   << "[HELLO]" << RES << std::endl;

    // Reversed background + bold text
    std::cout << "\nReversed Background + Bold Text:" << std::endl;
    std::cout << REV_BLCK  << "[HELLO]" << RES << std::endl;
    std::cout << REV_RED   << "[HELLO]" << RES << std::endl;
    std::cout << REV_WHITE << "[HELLO]" << RES << std::endl;
    std::cout << REV_BLUE  << "[HELLO]" << RES << std::endl;
    std::cout << REV_GREEN  << "[HELLO]" << RES << std::endl;
    std::cout << REV_CYAN  << "[HELLO]" << RES << std::endl;

    return 0;
}
