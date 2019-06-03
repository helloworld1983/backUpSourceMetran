/*
 * AudioMgr_Test.cpp
 *
 *  Created on: Feb 1, 2018
 *      Author: WINDOWS
 */

#include "stdafx.h"
#include "AudioMgr.c"


struct TestCaseList
{
	uint16_t volumeValue;
	E_ID_AudioVolume volume;
};

TestCaseList MyTestCaseList[5] =
{
        {6,eAudioVolumeLevel1},
        {7,eAudioVolumeLevel2},
        {8,eAudioVolumeLevel3},
        {9,eAudioVolumeLevel4},
        {10,eAudioVolumeLevel5},
};


class TestAudioMgr_GetVolume : public ::testing::TestWithParam<TestCaseList>
{
 protected:
  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
    // Code here will be called immediately after each test
    // (right before the destructor).
  }
};


TEST_P(TestAudioMgr_GetVolume, audioMgr_GetVolume)
{
	uint16_t volumeValue = GetParam().volumeValue;
	E_ID_AudioVolume volume = GetParam().volume;
    EXPECT_EQ(volumeValue, audioMgr_GetVolume(volume));
}

INSTANTIATE_TEST_CASE_P(audioMgr_GetVolume,TestAudioMgr_GetVolume, testing::ValuesIn(MyTestCaseList));





struct TestCaseList_InitBuffer
{
	uint32_t remain_data;
	unsigned char *audio;
	size_t size;
};


TestCaseList_InitBuffer MyTestCaseList_InitBuffer[5] =
{
        {DATA_SIZE_VOICE_ALERT_CANNULA - BUFFER_SIZE, audioVoiceAlertCannula, DATA_SIZE_VOICE_ALERT_CANNULA},
        {DATA_SIZE_VOICE_ALERT_CHARGE - BUFFER_SIZE, audioVoiceAlertCharge, DATA_SIZE_VOICE_ALERT_CHARGE},
        {DATA_SIZE_VOICE_ALERT_CONTACT - BUFFER_SIZE, audioVoiceAlertContact, DATA_SIZE_VOICE_ALERT_CONTACT},
        {DATA_SIZE_VOICE_CONT_FLOW010 - BUFFER_SIZE, audioVoiceContFlow010, DATA_SIZE_VOICE_CONT_FLOW010},
        {DATA_SIZE_VOICE_CONT_FLOW025 - BUFFER_SIZE, audioVoiceContFlow025, DATA_SIZE_VOICE_CONT_FLOW025},
};


class TestAudioMgr_InitBuffer: public ::testing::TestWithParam<TestCaseList_InitBuffer>
{
        virtual void SetUp()
        {
        }
        virtual void TearDown()
        {
        }

};

TEST_P(TestAudioMgr_InitBuffer, audioMgrInitBuffer)
{
	uint32_t remain_data = GetParam().remain_data;
	unsigned char *audio = GetParam().audio;
	size_t size = GetParam().size;
	EXPECT_EQ(remain_data, audioMgr_InitBuffer(audio,size));
}

INSTANTIATE_TEST_CASE_P(audioMgrInitBuffer,TestAudioMgr_InitBuffer, testing::ValuesIn(MyTestCaseList_InitBuffer));



