﻿#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>
#include <ConsoleLogger.h>
#include <StopWatch.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
void OnRender(IAudioObject* sender, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t renderFrameCount);
double PrintDeltaTime(const char* label);

Audio* audio;
int main()
{
	StopWatch::Start();
	audio = new Audio();
	audio->SetOnExceptionHandler(OnException);

	StopWatch::Reset();
	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	PrintDeltaTime("render initialized in");

	std::shared_ptr<IAudioObject> pao = audio->Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav");
	pao->OnRender = OnRender;
	pao->loopCount = 1u;
	PrintDeltaTime("file loaded in");

	pao->pause = false;

	std::string a;
	std::cin >> a;
	delete audio;
	pao = nullptr;
	StopWatch::Stop();
	std::cin >> a;

	return 0;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::string str = ex.ToString();
	size_t pos = str.find('\n', 0);
	str.insert(pos + 1, 21, ' ');
	pos = str.find('\n', pos + 1);
	str.insert(pos + 1, 21, ' ');
	ConsoleLogger::Log(str.c_str(), ConsoleLogger::error);
}
void OnRender(IAudioObject* sender, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t renderFrameCount)
{
}
double PrintDeltaTime(const char* label)
{
	const double dt = StopWatch::DeltaTime(StopWatch::milli);
	std::string message = label;
	message += " ";
	std::ostringstream dts;
	dts.precision(4);
	dts << std::fixed << dt;
	message += dts.str();
	message += " ms";
	ConsoleLogger::LogLine(message.c_str(), ConsoleLogger::info);
	StopWatch::Reset();
	return dt;
}