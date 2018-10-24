#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>

#include <QString>

using namespace std;

const char VIZ_OPTIONSTAG[2] = {'v','o'};
const char VIZ_LAYOUTTAG[2] = {'v','l'};

void showMessage(QString text);
bool isValidFormat(QString fmt);

int randbyte();
