#include "carnettype.h"

CarnetType::CarnetType() :
    multi(false),
    id(-1),
    hidden(false),
    enabled(true)
{
}

bool CarnetType::isValid()
{
    return id >= 0;
}
