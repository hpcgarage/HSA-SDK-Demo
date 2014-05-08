#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <inttypes.h>
#include <intrin.h>

#include "hsa.h"

uint8_t hsa_code_section[] = {
#include "demo.code.h"
};

uint8_t hsa_directives_section[] = {
#include "demo.directives.h"
};

uint8_t hsa_operands_section[] = {
#include "demo.operands.h"
};

uint8_t hsa_strtab_section[] = {
#include "demo.strtab.h"
};

void main() {
	HMODULE newhsacore = ::LoadLibraryW(L"newhsacore64.dll");
	assert(newhsacore != NULL);

	HsaGetDevicesFunction HsaGetDevices = reinterpret_cast<HsaGetDevicesFunction>(::GetProcAddress(newhsacore, "HsaGetDevices"));
	assert(HsaGetDevices != NULL);
	HsaCreateUserModeQueueFunction HsaCreateUserModeQueue = reinterpret_cast<HsaCreateUserModeQueueFunction>(::GetProcAddress(newhsacore, "HsaCreateUserModeQueue"));
	assert(HsaCreateUserModeQueue != NULL);
	HsaDestroyUserModeQueueFunction HsaDestroyUserModeQueue = reinterpret_cast<HsaDestroyUserModeQueueFunction>(::GetProcAddress(newhsacore, "HsaDestroyUserModeQueue"));
	assert(HsaDestroyUserModeQueue != NULL);
	HsaSubmitAqlFunction HsaSubmitAql = reinterpret_cast<HsaSubmitAqlFunction>(::GetProcAddress(newhsacore, "HsaSubmitAql"));
	assert(HsaSubmitAql != NULL);

	HsaCreateSignalFunction HsaCreateSignal = reinterpret_cast<HsaCreateSignalFunction>(::GetProcAddress(newhsacore, "HsaCreateSignal"));
	assert(HsaCreateSignal != NULL);
	HsaDestroySignalFunction HsaDestroySignal = reinterpret_cast<HsaDestroySignalFunction>(::GetProcAddress(newhsacore, "HsaDestroySignal"));
	assert(HsaDestroySignal != NULL);
	HsaWaitOnSignalFunction HsaWaitOnSignal = reinterpret_cast<HsaWaitOnSignalFunction>(::GetProcAddress(newhsacore, "HsaWaitOnSignal"));
	assert(HsaWaitOnSignal != NULL);
	HsaQuerySignalFunction HsaQuerySignal = reinterpret_cast<HsaQuerySignalFunction>(::GetProcAddress(newhsacore, "HsaQuerySignal"));
	assert(HsaQuerySignal != NULL);

	HsaLoadBrigFunction HsaLoadBrig = reinterpret_cast<HsaLoadBrigFunction>(::GetProcAddress(newhsacore, "HsaLoadBrig"));
	assert(HsaLoadBrig != NULL);
	HsaUnloadBrigFunction HsaUnloadBrig = reinterpret_cast<HsaUnloadBrigFunction>(::GetProcAddress(newhsacore, "HsaUnloadBrig"));
	assert(HsaUnloadBrig != NULL);
	HsaFinalizeBrigFunction HsaFinalizeBrig = reinterpret_cast<HsaFinalizeBrigFunction>(::GetProcAddress(newhsacore, "HsaFinalizeBrig"));
	assert(HsaFinalizeBrig != NULL);
	HsaFreeKernelCodeFunction HsaFreeKernelCode = reinterpret_cast<HsaFreeKernelCodeFunction>(::GetProcAddress(newhsacore, "HsaFreeKernelCode"));
	assert(HsaFreeKernelCode != NULL);
	HsaFreeKernelDebugFunction HsaFreeKernelDebug = reinterpret_cast<HsaFreeKernelDebugFunction>(::GetProcAddress(newhsacore, "HsaFreeKernelDebug"));
	assert(HsaFreeKernelDebug != NULL);

	HsaRegisterSystemMemoryFunction HsaRegisterSystemMemory = reinterpret_cast<HsaRegisterSystemMemoryFunction>(::GetProcAddress(newhsacore, "HsaRegisterSystemMemory"));
	assert(HsaRegisterSystemMemory != NULL);
	HsaDeregisterSystemMemoryFunction HsaDeregisterSystemMemory = reinterpret_cast<HsaDeregisterSystemMemoryFunction>(::GetProcAddress(newhsacore, "HsaDeregisterSystemMemory"));
	assert(HsaDeregisterSystemMemory != NULL);

	HsaStatus status = kHsaStatusSuccess;
	const HsaDevice* device = NULL;
	unsigned int deviceCount = 0;

	status = HsaGetDevices(&deviceCount, &device);
	assert(status == kHsaStatusSuccess);
	assert(deviceCount == 1);

	HsaBrig brig;
	memset(&brig, 0, sizeof(brig));
	brig.code_section = hsa_code_section;
	brig.code_section_byte_size = sizeof(hsa_code_section);
	brig.directive_section = hsa_directives_section;
	brig.directive_section_byte_size = sizeof(hsa_directives_section);
	brig.operand_section = hsa_operands_section;
	brig.operand_section_byte_size = sizeof(hsa_operands_section);
	brig.string_section = hsa_strtab_section;
	brig.string_section_byte_size = sizeof(hsa_strtab_section);
	status = HsaLoadBrig(device, &brig);
	assert(status == kHsaStatusSuccess);

	HsaKernelCode *kernelCode = NULL;
	status = HsaFinalizeBrig(device, &brig, "&hsaDemo", "", &kernelCode, NULL);
	assert(status == kHsaStatusSuccess);
	assert(kernelCode != NULL);

	status = HsaUnloadBrig(&brig);
	assert(status == kHsaStatusSuccess);

	HsaQueue* queue = NULL;
	status = HsaCreateUserModeQueue(device, NULL, 0, kHsaQueueTypeCompute, kHsaQueuePriorityMaximum, kHsaQueueFractionTen, &queue);
	assert(status == kHsaStatusSuccess);

	HsaSignal signal = NULL;
	status = HsaCreateSignal(&signal);
	assert(status == kHsaStatusSuccess);

	for (size_t arraySize = 64; arraySize <= 1024 * 1024; arraySize *= 2) {
		uint32_t* xArray = (uint32_t*)::VirtualAlloc(NULL, arraySize * sizeof(uint32_t), MEM_RESERVE, PAGE_READWRITE);
		assert(xArray != NULL);
		xArray = (uint32_t*)::VirtualAlloc(xArray, arraySize * sizeof(uint32_t), MEM_COMMIT, PAGE_READWRITE);
		assert(xArray != NULL);
		memset(xArray, 0x12, arraySize * sizeof(uint32_t));

		uint32_t* yArray = (uint32_t*)::VirtualAlloc(NULL, arraySize * sizeof(uint32_t), MEM_RESERVE, PAGE_READWRITE);
		assert(yArray != NULL);
		yArray = (uint32_t*)::VirtualAlloc(yArray, arraySize * sizeof(uint32_t), MEM_COMMIT, PAGE_READWRITE);
		assert(yArray != NULL);
		memset(yArray, 0x14, arraySize * sizeof(uint32_t));

		uint32_t* zArray = (uint32_t*)::VirtualAlloc(NULL, arraySize * sizeof(uint32_t), MEM_RESERVE, PAGE_READWRITE);
		assert(zArray != NULL);
		zArray = (uint32_t*)::VirtualAlloc(zArray, arraySize * sizeof(uint32_t), MEM_COMMIT, PAGE_READWRITE);
		assert(zArray != NULL);
		memset(zArray, 0x42, arraySize * sizeof(uint32_t));

		for (size_t iteration = 1; iteration <= 5; iteration++) {
			uint64_t kernelArguments[4];
			kernelArguments[0] = reinterpret_cast<uint64_t>(xArray);
			kernelArguments[1] = reinterpret_cast<uint64_t>(yArray);
			kernelArguments[2] = reinterpret_cast<uint64_t>(zArray);
			kernelArguments[3] = static_cast<uint64_t>(arraySize);

			memset(zArray, 0x42, arraySize * sizeof(uint32_t));

			HsaAqlDispatchPacket aqlPacket;
			memset(&aqlPacket, 0, sizeof(aqlPacket));
			aqlPacket.format = kHsaAqlPacketFormatDispatch;
			aqlPacket.invalidate_instruction_cache = 1;
            /* This is very important. Without release fence the system will crash from time to time. */
			aqlPacket.release_fence_scope = 2;
			aqlPacket.dimensions = 1;
			aqlPacket.grid_size[0] = static_cast<uint32_t>(arraySize);
			aqlPacket.grid_size[1] = 1;
			aqlPacket.grid_size[2] = 1;
			aqlPacket.workgroup_size[0] = device->wave_front_size * device->number_compute_units;
			aqlPacket.workgroup_size[1] = 1;
			aqlPacket.workgroup_size[2] = 1;
			aqlPacket.completion_signal = signal;
			aqlPacket.group_segment_size_bytes = kernelCode->workgroup_group_segment_byte_size;
			aqlPacket.private_segment_size_bytes = kernelCode->workitem_private_segment_byte_size;
			aqlPacket.kernel_object_address = reinterpret_cast<uint64_t>(kernelCode);
			aqlPacket.kernel_arg_address = reinterpret_cast<uint64_t>(kernelArguments);

			const uint64_t gpuStartCycles = __rdtsc();
			status = HsaSubmitAql(queue, &aqlPacket);
			assert(status == kHsaStatusSuccess);
			const uint64_t gpuSubmitCycles = __rdtsc();

			status = HsaWaitOnSignal(signal);
			assert(status == kHsaStatusSuccess);
			const uint64_t gpuComputeCycles = __rdtsc();

			size_t countEqual = 0;
			for (size_t i = 0; i < arraySize; i++) {
				if (xArray[i] + yArray[i] == zArray[i])
					countEqual++;
				else if (i < 10)
					printf("%"PRIx32" + %"PRIx32" = %"PRIx32"\n", xArray[i], yArray[i], zArray[i]);
			}
			if (countEqual != arraySize) {
				printf("%Iu\tFAILED (%Iu)\n", arraySize, countEqual);
				break;
			}

			const uint64_t cpuStartCycles = __rdtsc();
			for (size_t i = 0; i < arraySize; i += 16) {
				_mm_stream_si128((__m128i*)&zArray[i], _mm_add_epi32(_mm_load_si128((const __m128i*)&xArray[i]), _mm_load_si128((const __m128i*)&yArray[i])));
				_mm_stream_si128((__m128i*)&zArray[i + 4], _mm_add_epi32(_mm_load_si128((const __m128i*)&xArray[i + 4]), _mm_load_si128((const __m128i*)&yArray[i + 4])));
				_mm_stream_si128((__m128i*)&zArray[i + 8], _mm_add_epi32(_mm_load_si128((const __m128i*)&xArray[i + 8]), _mm_load_si128((const __m128i*)&yArray[i + 8])));
				_mm_stream_si128((__m128i*)&zArray[i + 12], _mm_add_epi32(_mm_load_si128((const __m128i*)&xArray[i + 12]), _mm_load_si128((const __m128i*)&yArray[i + 12])));
			}
			_mm_sfence();
			const uint64_t cpuComputeCycles = __rdtsc();

			const double gpuSubmitTime = double(gpuSubmitCycles - gpuStartCycles) / 3.7e+9;
			const double gpuComputeTime = double(gpuComputeCycles - gpuStartCycles) / 3.7e+9;
			const double cpuComputeTime = double(cpuComputeCycles - cpuStartCycles) / 3.7e+9;
			const double gpuBandwidth = double(arraySize) * double(3 * sizeof(uint32_t)) / gpuComputeTime;
			const double cpuBandwidth = double(arraySize) * double(3 * sizeof(uint32_t)) / cpuComputeTime;
			printf("%Iu\t%.2lf\t%.2lf\t%.2lf\t%.3lf\t%.3lf\n", arraySize, gpuSubmitTime * 1.0e+6, gpuComputeTime * 1.0e+6, cpuComputeTime * 1.0e+6, gpuBandwidth * 1.0e-9, cpuBandwidth * 1.0e-9);
		}

		::VirtualFree(xArray, 0, MEM_DECOMMIT);
		::VirtualFree(xArray, 0, MEM_RELEASE);
		::VirtualFree(yArray, 0, MEM_DECOMMIT);
		::VirtualFree(yArray, 0, MEM_RELEASE);
		::VirtualFree(zArray, 0, MEM_DECOMMIT);
		::VirtualFree(zArray, 0, MEM_RELEASE);
	}

	status = HsaDestroySignal(signal);
	assert(status == kHsaStatusSuccess);

	status = HsaFreeKernelCode(kernelCode);
	assert(status == kHsaStatusSuccess);

	status = HsaDestroyUserModeQueue(queue);
	assert(status == kHsaStatusSuccess);

	BOOL freeHsaCore = ::FreeLibrary(newhsacore);
	assert(freeHsaCore != FALSE);
}
