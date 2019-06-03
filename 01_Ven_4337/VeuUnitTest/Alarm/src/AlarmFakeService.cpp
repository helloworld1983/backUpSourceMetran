#include "AlarmFixture.h"
#include "AlarmFakeService.h"

void Enunciate(int i)
{
    return AlarmTestFixture::_AlarmControlMocks->Enunciate(i);
}
