#pragma once
#include "VK.h"
#include "Win32.h"
#include "DynamicVertexBuffer.h"
#include "TextRenderer.h"
#include "WASAPIInterface.h"
#include "SerializeTools.h"
#include "UI.h"
#include "PanelUI.h"
#include "stb_vorbis.h"


namespace CyberSeaquell {

U64 frameNumber;
U64 prevFrameTime;
U64 frameTime;
F64 deltaTime;
F64 totalTime;

HANDLE audioThread;
F64 audioPlaybackTime;
B32 audioThreadShouldShutdown;

F32* testAudio;
U32 testAudioLength;

void fill_audio_buffer(F32* buffer, U32 numSamples, U32 numChannels, F32 timeAmount) {
	for (U32 i = 0; i < numSamples; i++) {
		F64 t = audioPlaybackTime + F64(i) / F64(numSamples) * F64(timeAmount);
		F32 val = testAudio[U32(t * 44100.0) % testAudioLength];
		for (U32 j = 0; j < numChannels; j++) {
			*buffer++ = val * 1.0F;
		}
	}
	audioPlaybackTime += timeAmount;
}

DWORD WINAPI audio_thread_func(LPVOID) {
	int channels, sampleRate;
	short* output;
	int dataLength = stb_vorbis_decode_filename("./resources/sounds/seagulls.ogg", &channels, &sampleRate, &output);
	testAudioLength = dataLength;
	testAudio = globalArena.alloc<F32>(dataLength);
	for (U32 i = 0; i < dataLength; i++) {
		testAudio[i] = F32(output[i * 2]) / 65535.0F;
	}
	WASAPIInterface::init_wasapi(fill_audio_buffer);
	while (!audioThreadShouldShutdown) {
		WASAPIInterface::do_audio();
	}
	return 0;
}

void keyboard_callback(Win32::Key key, Win32::ButtonState state) {
	V2F32 mousePos = Win32::get_mouse();
	UI::handle_keyboard_action(mousePos, key, state);
}
void mouse_callback(Win32::MouseButton button, Win32::MouseValue state) {
	V2F32 mousePos = Win32::get_mouse();
	UI::handle_mouse_action(mousePos, button, state);
}

void do_frame() {
	frameNumber++;
	LARGE_INTEGER perfCounter;
	if (!QueryPerformanceCounter(&perfCounter)) {
		abort("Could not get performance counter");
	}
	prevFrameTime = frameTime;
	frameTime = U64(perfCounter.QuadPart);
	deltaTime = F64(frameTime - prevFrameTime) / F64(performanceCounterTimerFrequency);
	totalTime += deltaTime;

	V2F32 mousePos = Win32::get_mouse();
	V2F32 mouseDelta = Win32::get_delta_mouse();
	UI::handle_mouse_update(mousePos, mouseDelta);

	VK::FrameBeginResult beginAction = VK::begin_frame();
	if (beginAction == VK::FRAME_BEGIN_RESULT_TRY_AGAIN) {
		beginAction = VK::begin_frame();
	}
	if (beginAction == VK::FRAME_BEGIN_RESULT_CONTINUE) {
		UI::layout_boxes(VK::desktopSwapchainData.width, VK::desktopSwapchainData.height);

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = F32(VK::mainFramebuffer.framebufferWidth);
		viewport.height = F32(VK::mainFramebuffer.framebufferHeight);
		viewport.minDepth = 0.0F;
		viewport.maxDepth = 1.0F;
		VK::vkCmdSetViewport(VK::graphicsCommandBuffer, 0, 1, &viewport);
		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = VK::mainFramebuffer.framebufferWidth;
		scissor.extent.height = VK::mainFramebuffer.framebufferHeight;
		VK::vkCmdSetScissor(VK::graphicsCommandBuffer, 0, 1, &scissor);

		VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassBeginInfo.renderPass = VK::mainRenderPass;
		renderPassBeginInfo.framebuffer = VK::mainFramebuffer.framebuffer;
		renderPassBeginInfo.renderArea = VkRect2D{ VkOffset2D{ 0, 0 }, VkExtent2D{ VK::mainFramebuffer.framebufferWidth, VK::mainFramebuffer.framebufferHeight } };
		renderPassBeginInfo.clearValueCount = 2;
		VkClearValue clearValues[2];
		clearValues[0].color.float32[0] = 0.0F;
		clearValues[0].color.float32[1] = 0.0F;
		clearValues[0].color.float32[2] = 0.0F;
		clearValues[0].color.float32[3] = 0.0F;
		clearValues[1].depthStencil.depth = 0.0F;
		clearValues[1].depthStencil.stencil = 0;
		renderPassBeginInfo.pClearValues = clearValues;
		VK::vkCmdBeginRenderPass(VK::graphicsCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		UI::draw();
		DynamicVertexBuffer::get_tessellator().draw();

		VK::vkCmdEndRenderPass(VK::graphicsCommandBuffer);

		VK::end_frame();
	} else {
		Sleep(1);
	}
}

void run_cyber_seaquell() {
	audioThread = CreateThread(NULL, 64 * KILOBYTE, audio_thread_func, NULL, 0, NULL);
	if (audioThread == NULL) {
		DWORD err = GetLastError();
		print("Failed to create audio thread, code: ");
		println_integer(err);
		return;
	}

	LARGE_INTEGER perfCounter;
	if (!QueryPerformanceCounter(&perfCounter)) {
		abort("Could not get performanceCounter");
	}
	frameTime = prevFrameTime = U64(perfCounter.QuadPart);

	if (!Win32::init(1920 / 2, 1080 / 2, do_frame, keyboard_callback, mouse_callback)) {
		return;
	}
	PNG::init_loader();
	LOG_TIME("VK Init Time: ") {
		VK::init_vulkan();
	}
	LOG_TIME("Asset Load Time: ") {
		VK::load_pipelines_and_descriptors();
		Textures::load_all();
		VK::finish_startup();
	}

	UI::init_ui();
	PanelUI::init();

	Win32::show_window();

	while (!Win32::windowShouldClose) {
		Win32::poll_events();
		do_frame();
	}

	audioThreadShouldShutdown = true;
	if (WaitForSingleObject(audioThread, INFINITE) == WAIT_FAILED) {
		DWORD err = GetLastError();
		print("Failed to join audio thread, code: ");
		println_integer(err);
	} else {
		CloseHandle(audioThread);
	}
	CHK_VK(VK::vkDeviceWaitIdle(VK::logicalDevice));
	UI::destroy_ui();
	VK::end_vulkan();
	Win32::destroy();
}

}