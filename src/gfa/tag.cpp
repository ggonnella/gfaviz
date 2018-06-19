#include "gfa/tag.h"
#include "gfa/error.h"
#include <string.h>

GfaTag::GfaTag(char* str, unsigned long len) {
  //TODO: Validation
  if (len < 6 || str[2] != ':' || str[4] != ':') {
    throw fatal_error() << "Malformed tag '" << string(str,len) << "'.";
  }
  key[0] = str[0];
  key[1] = str[1];
  switch (str[3]) {
    case GFA_TAG_CHAR: {
      val_char = str[5];
      break;
    }
    case GFA_TAG_INT: {
      val_long = atol(str+5);
      break;
    }
    case GFA_TAG_FLOAT: {
      val_float = atof(str+5);
      break;
    }
    case GFA_TAG_STRING:
    case GFA_TAG_JSON:
    case GFA_TAG_BYTEARRAY:
    case GFA_TAG_INTARRAY: {
      val_string = strndup(str+5, len-5);
      break;
    }
    default: {
      throw fatal_error() << "Unknown tag format specifier '" << str[3] << "' in tag '" << string(str,len) << "'.";
    }    
  }
  type = (GfaTagType)str[3];  
}

GfaTag::~GfaTag() {
  
}

void GfaTag::print() const {
  cout << key[0] << key[1] << ':' << (char)type << ':';
  switch (type) {
    case GFA_TAG_CHAR: {
      cout << val_char;
      break;
    }
    case GFA_TAG_INT: {
      cout << val_long;
      break;
    }
    case GFA_TAG_FLOAT: {
      cout << val_float;
      break;
    }
    case GFA_TAG_STRING:
    case GFA_TAG_JSON:
    case GFA_TAG_BYTEARRAY:
    case GFA_TAG_INTARRAY: {
      cout << val_string;
      break;
    }
    case GFA_TAG_WILDCARD: {
      throw fatal_error() << "Tried to print empty tag.";
    }
  }
}

int16_t GfaTag::getID() const {
  return id;
}

GfaTagType GfaTag::getType() const {
  return type;
}

long GfaTag::getIntValue() {
  if (type != GFA_TAG_INT) {
    throw fatal_error() << "Tried to read integer value from non-integer tag '" << key[0] << key[1] << "'.";
  }
  return val_long;
}
char* GfaTag::getStringValue() {
  if (type != GFA_TAG_STRING && type != GFA_TAG_JSON) {
    throw fatal_error() << "Tried to read string value from non-string tag '" << key[0] << key[1] << "'.";
  }
  return val_string;
}
