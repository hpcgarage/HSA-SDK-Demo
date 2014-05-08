#pragma once

#include <stdint.h>

#define HSACORE_CPU_SIBLINGS 256

enum HsaStatus {
	kHsaStatusSuccess = 0,
	kHsaStatusTimedOut = 1,
	kHsaStatusInvalidArgument = -1,
	kHsaStatusOutOfResources = -2,
	kHsaStatusResourceUnavailable = -3,
	kHsaStatusInvalidResource = -4,
	kHsaStatusUnsupportedOperation = -5,
	kHsaStatusInvalidOperation = -6,
	kHsaStatusBuildFailed = -7,
	kHsaStatusError = -15,
	kHsaStatusNotImplemented = -16,
	kHsaStatusNotInitialized = -17,
	kHsaStatusInitFailed = -18,
	kHsaStatusMisaligned = -19,
	kHsaStatusRetry = -22,
	kHsaStatusOutOfMemory = -23
};

enum HsaHeapType {
	kHsaHeapTypeSystem = 0,
	kHsaHeapTypeFrameBufferPublic = 1,
	kHsaHeapTypeFrameBufferPrivate = 2,
	kHsaHeapTypeNumHeapTypes,
	kHsaHeapTypeSize = 0xFFFFFFFF
};

enum HsaDeviceType {
	kHsaDeviceTypeThroughput = 1
};

enum HsaCacheLevel {
	kHsaCacheLevelL1Cache = 0x1,
	kHsaCacheLevelL2Cache = 0x2,
	kHsaCacheLevelConstantCache = 0x4,
	kHsaCacheLevelTextureCache = 0x8
};

enum HsaSignalCondition {
	kHsaSignalConditionDebug,
	kHsaSignalConditionSyscall
};

enum HsaQueueType {
	kHsaQueueTypeCompute = 0,
	kHsaQueueTypeInterop = 1
};

enum HsaQueuePriority {
	kHsaQueuePriorityMinimum = -3,
	kHsaQueuePriorityLow = -2,
	kHsaQueuePriorityBelowNormal = -1,
	kHsaQueuePriorityNormal = 0,
	kHsaQueuePriorityAboveNormal = 1,
	kHsaQueuePriorityHigh = 2,
	kHsaQueuePriorityMaximum = 3
};

enum HsaQueueFraction {
	kHsaQueueFractionOne = 10,
	kHsaQueueFractionTwo = 20,
	kHsaQueueFractionThree = 30,
	kHsaQueueFractionFour = 40,
	kHsaQueueFractionFive = 50,
	kHsaQueueFractionSix = 60,
	kHsaQueueFractionSeven = 70,
	kHsaQueueFractionEight = 80,
	kHsaQueueFractionNine = 90,
	kHsaQueueFractionTen = 100
};

union HsaCoreCacheType {
	uint32_t value;
	struct {
		unsigned data        :  1;
		unsigned instruction :  1;
		unsigned cpu         :  1;
		unsigned hsa_cu      :  1;
		unsigned reserved    : 28;
	} ui32;
};

struct HsaCacheDescriptor {
	uint32_t processor_id_low;
	enum HsaCacheLevel cache_level;
	uint32_t cache_size;
	uint32_t cache_line_size;
	uint32_t cache_lines_per_tag;
	uint32_t cache_associativity;
	uint32_t cache_latency;
	union HsaCoreCacheType cache_type;
	uint32_t sibling_map[HSACORE_CPU_SIBLINGS];
};

union HsaMemoryProperty {
	uint32_t memory_property;
	struct {
		unsigned int hot_pluggable : 1;
		unsigned int non_volatile : 1;
		unsigned int reserved : 30;
	} ui32;
};

struct HsaMemoryDescriptor {
	enum HsaHeapType heap_type;
	union {
		uint64_t size_in_bytes;
		struct {
			uint32_t size_in_bytes_low;
			uint32_t size_in_bytes_high;
		} ui32;
	};
	union HsaMemoryProperty flags;
	uint64_t virtual_base_address;
	uint32_t width;
	uint32_t memory_clock_max;
};

typedef struct {
	uint32_t device_id;
	enum HsaDeviceType device_type;
	uint32_t node_id;
	uint32_t location_id;
	uint32_t number_compute_units;
	uint32_t max_clock_rate_of_f_compute;
	uint32_t max_clock_rate_of_c_compute;
	uint32_t group_memory_size;
	uint32_t wave_front_size;
	uint32_t max_waves_per_simd;
	uint8_t is_dedicated_compute;
	uint32_t number_memory_descriptors;
	struct HsaMemoryDescriptor* memory_descriptors;
	uint32_t number_cache_descriptors;
	struct HsaCacheDescriptor* cache_descriptors;
	uint32_t queue_size;
	uint8_t is_image_support;
	uint8_t is_double_precision;
	uint32_t HsaMaxConcurrentExecutions;
	uint32_t max_address_watch_points;
	char *device_name;
} HsaDevice;

struct HsaBrig {
	void* string_section;
	void* directive_section;
	void* code_section;
	void* operand_section;
	void* debug_section;
	void* loadmap_section;
	void* global_variables_base;
	size_t string_section_byte_size;
	size_t directive_section_byte_size;
	size_t code_section_byte_size;
	size_t operand_section_byte_size;
	size_t debug_section_byte_size;
	size_t loadmap_section_byte_size;
};

enum HsaAqlPacketFormat {
	kHsaAqlPacketFormatInvalid = 0,
	kHsaAqlPacketFormatDispatch = 1,
	kHsaAqlPacketFormatDepend = 2
};

typedef uint64_t HsaSignal;

typedef struct {
	uint32_t format                       :  8;
	uint32_t barrier                      :  1;
	uint32_t acquire_fence_scope          :  2;
	uint32_t release_fence_scope          :  2;
	uint32_t invalidate_instruction_cache :  1;
	uint32_t invalidate_roi_image_cache   :  1;
	uint32_t dimensions                   :  2;
	uint32_t reserved                     : 15;
	uint16_t workgroup_size[3];
	uint16_t reserved2;
	uint32_t grid_size[3];
	uint32_t private_segment_size_bytes;
	uint32_t group_segment_size_bytes;
	uint64_t kernel_object_address;
	uint64_t kernel_arg_address;
	uint64_t reserved3;
	HsaSignal completion_signal;
} HsaAqlDispatchPacket;

typedef struct {
	uint32_t format                       : 8;
	uint32_t barrier                      : 1;
	uint32_t acquire_fence_scope          : 2;
	uint32_t release_fence_scope          : 2;
	uint32_t invalidate_instruction_cache : 1;
	uint32_t invalidate_roi_image_cache   : 1;
	uint32_t dimensions                   : 2;
	uint32_t reserved                     : 15;
	uint32_t reserved2;
	HsaSignal dep_signal0;
	HsaSignal dep_signal1;
	HsaSignal dep_signal2;
	HsaSignal dep_signal3;
	HsaSignal dep_signal4;
	uint64_t reserved3;
	HsaSignal completion_signal;
} HsaAqlBarrierPacket;

static_assert(sizeof(HsaAqlBarrierPacket) == 64, "WRONG AQL BARRIER PACKET");

typedef struct {
	uint64_t queue_id;
	const HsaDevice* device;
	uint8_t* base;
	size_t size_bytes;
	volatile uint32_t *cached_write_offset;
	volatile uint32_t *write_offset;
	volatile uint32_t *read_offset;
	volatile uint32_t *doorbell_offset;
	uint64_t dispatch_id;
	void* queue_details;
} HsaQueue;

typedef uint16_t HsaExceptionKind16_t;

struct HsaControlDirectives {
	uint16_t enabled_control_directives;
	uint8_t required_dim;
	uint8_t require_no_partial_workgroups: 1;
	uint8_t reserved1: 7;
	HsaExceptionKind16_t enable_break_exceptions;
	HsaExceptionKind16_t enable_detect_exceptions;
	uint32_t max_flat_grid_size;
	uint16_t required_grid_size[3];
	uint32_t max_flat_workgroup_size;
	uint16_t required_workgroup_size[3];
	uint32_t requested_workgroups_per_cu;
	uint32_t max_dynamic_group_size;
	uint32_t reserved2[4];
};


typedef uint8_t BrigProfile8_t;
typedef uint8_t BrigMachineModel8_t;
typedef uint16_t BrigTargetOptions16_t;

struct HsaKernelCode {
	uint64_t size;
	char device_vendor[16];
	char device_name[16];
	uint32_t hsail_version_major;
	uint32_t hsail_version_minor;
	BrigProfile8_t hsail_profile;
	BrigMachineModel8_t hsail_machine_model;
	BrigTargetOptions16_t hsail_target_options;
	uint64_t kernarg_segment_byte_size;
	uint32_t workgroup_group_segment_byte_size;
	uint32_t workitem_private_segment_byte_size;
	uint32_t workitem_spill_segment_byte_size;
	uint32_t workitem_arg_segment_byte_size;
	uint32_t workgroup_fbarrier_count;
	struct HsaControlDirectives control_directive;
	uint8_t wavefront_size;
	uint8_t optimization_level;
	unsigned char reserved[14];
};

typedef void* HsaKernelDebug;

typedef HsaStatus (*HsaCreateUserModeQueueFunction)(const HsaDevice *device, void *buffer, size_t buffer_size, HsaQueueType queue_type, HsaQueuePriority queue_priority, HsaQueueFraction queue_fraction, HsaQueue **queue);
typedef HsaStatus (*HsaDestroyUserModeQueueFunction)(HsaQueue *queue);
typedef HsaStatus (*HsaGetDevicesFunction)(unsigned int *count, const HsaDevice **device_list);
typedef HsaStatus (*HsaLoadBrigFunction)(const HsaDevice *device, HsaBrig *brig);
typedef HsaStatus (*HsaUnloadBrigFunction)(HsaBrig *brig);
typedef HsaStatus (*HsaFinalizeBrigFunction)(const HsaDevice *device, HsaBrig *brig, const char *kernel_name, const char *options, HsaKernelCode **kernel, HsaKernelDebug **debug);
typedef HsaStatus (*HsaFreeKernelCodeFunction)(HsaKernelCode *kernel);
typedef HsaStatus (*HsaFreeKernelDebugFunction)(HsaKernelDebug *debug);
typedef HsaStatus (*HsaCreateSignalFunction)(HsaSignal *signal);
typedef HsaStatus (*HsaDestroySignalFunction)(HsaSignal signal);
typedef HsaStatus (*HsaQuerySignalFunction)(HsaSignal signal, uint8_t* is_done);
typedef HsaStatus (*HsaWaitOnSignalFunction)(HsaSignal signal);
typedef HsaStatus (*HsaSubmitAqlFunction)(HsaQueue *queue, HsaAqlDispatchPacket *aql_packet);
typedef HsaStatus (*HsaRegisterSystemMemoryFunction)(void *address, size_t size);
typedef HsaStatus (*HsaDeregisterSystemMemoryFunction)(void *address);
typedef HsaStatus (*HsaFreeKernelCodeFunction)(HsaKernelCode *kernel);
typedef HsaStatus (*HsaFreeKernelDebugFunction)(HsaKernelDebug *debug);
