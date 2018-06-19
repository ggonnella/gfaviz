#include "gfa/validate.h"
#include "gfa/defines.h"
#include <cctype>

bool isValidUword(const char *str, unsigned long len) {
  unsigned long idx;
  if (len == 0) {
    return false;
  }
  for (idx = 0; idx < len; idx++) {
    if (!isdigit(str[idx])) {
      return false;
    }
  }
  return true;
}

bool isValidWord(const char *str, unsigned long len) {
  unsigned long idx;

  if (len == 0 || (len == 1 && str[0] == '-') || (!isdigit(str[0]) && str[0] != '-' && str[0] != '+')) {
    return false;
  }
  for (idx = 1; idx < len; idx++) {
    if (!isdigit(str[idx])) {
      return false;
    }
  }
  return true;
}

bool isValidLabel(const char *str, unsigned long len)
{
  unsigned long idx;

  if (len == 0) {
    return false;
  }
  for (idx = 0; idx < len; idx++) {
    if (str[idx] < '!' || str[idx] > '~') {
      return false;
    }
  }
  return true;
}


bool isValidSegmentLabel(const char *str, unsigned long len)
{
  unsigned long idx;

  if (len == 0) {
    return false;
  }

  /* must not start with '*' or '=' */
  if (str[0] == '*' || str[0] == '=') {
    return false;
  }

  for (idx = 0; idx < len; idx++) {
    if (str[idx] < '!' || str[idx] > '~') {
      return false;
    }
    /* must not contain "+," or "-," */
    if ((idx + 1) < len && (str[idx] == '+' || str[idx] == '-')) {
      if (str[idx + 1] == ',') {
        return false;
      }
    }
  }
  return true;
}

bool isValidSequence(const char *str, unsigned long len)
{
  unsigned long idx;

  if (len == 1 && str[0] == '*') /* 1st alternative: '*' */
    return true;
  else {/* any printable sequence excluding space [!-~]+ */
    for (idx = 0; idx < len; idx++) {
      if (str[idx] < '!' || str[idx] > '~') {
        return false;
      }
    }
  }
  return true;
}

bool isValidOrientation(const char *str, unsigned long len) {
  return (len == 1 && (str[0] == GFA_REVERSE || str[0] == GFA_FORWARD));
}

bool isValidAlignment(const char *str, unsigned long len) {
  unsigned long idx;
  bool char_found = false;
  bool digit_found = false;
  if (len == 1 && str[0] == '*') /* 1st alternative: '*' */
    return true;
  else {/* Just allowing MIDP as of GFA2 */
    for (idx = 0; idx < len; idx++) {
      if ((str[idx] == 'M' || str[idx] == 'I' || str[idx] == 'D' || str[idx] == 'P') &&
          digit_found == true) { /* MIDP*/
        char_found = true;
        digit_found = false;
      } else if (isdigit(str[idx])) {
        digit_found = true;
        char_found = false;
      } else {
        return false;
      }
    }
  }
  if (!char_found) /* Dazzler Traces not compatible as of now */
    return false;
  else
    return true;
}

bool isValidPos(const char *str, unsigned long len)
{
  if (len == 0)
    return false;
  else if (str[len-1] == GFA_SENTINEL)
    return isValidUword(str, len - 1);
  else
    return isValidUword(str, len);
}
