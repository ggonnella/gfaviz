#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>

#include <QString>

using namespace std;

/* Short helper functions, useful in different contexts in the program.
   The function implementations are in helpers.cpp. */

/* Show a dialog box with a message. Used for error communication. */
void showMessage(QString text);

/* Check if the file format <fmt> (case-insensitive) is contained in a
   hard-coded list of supported image file formats. Note: The list includes
   SVG or not, depending on the compiler settings. */
bool isValidImageFormat(QString fmt);

/* A random positive integer between 0 and 255 (which could
   be stored in a byte, hence the function name). */
int randbyte();

/* A random positive integer between 0 and 359 (which can
   be used to specify a hue value, hence the function name) */
int randhue();

/* TODO: move these two to CLI options --options-tag and --layout-tag */
const char VIZ_OPTIONSTAG[2] = {'v','o'};
const char VIZ_LAYOUTTAG[2] = {'v','l'};

