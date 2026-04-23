#include <raymob.h>
void stopCallback()
{
	TraceLog(LOG_INFO, "Stopped");
	if (!saveForReload(&g_gameState)) {
		TraceLog(LOG_ERROR, "Couldn't saveForReload");
	}
}
void pauseCallback()
{
	TraceLog(LOG_INFO, "Paused");
	if (!saveForReload(&g_gameState)) {
		TraceLog(LOG_ERROR, "Couldn't saveForReload");
	}
}
void PlatformInits()
{
	InitCallBacks();
	SetOnStopCallBack(&stopCallback);
	SetOnPauseCallBack(&pauseCallback);
}
