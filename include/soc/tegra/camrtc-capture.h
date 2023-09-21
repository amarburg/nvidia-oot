/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2016-2023, NVIDIA CORPORATION & AFFILIATES.  All rights reserved.
 */

/**
 * @file camrtc-capture.h
 *
 * @brief Camera firmware API header
 */

#ifndef INCLUDE_CAMRTC_CAPTURE_H
#define INCLUDE_CAMRTC_CAPTURE_H

#include "camrtc-common.h"

#pragma GCC diagnostic error "-Wpadded"

#define CAPTURE_IVC_ALIGNOF		MK_ALIGN(8)
#define CAPTURE_DESCRIPTOR_ALIGN_BYTES (64)	/**< Descriptor alignment in shared memory */
#define CAPTURE_DESCRIPTOR_ALIGNOF	MK_ALIGN(CAPTURE_DESCRIPTOR_ALIGN_BYTES)

#define CAPTURE_IVC_ALIGN		CAMRTC_ALIGN(CAPTURE_IVC_ALIGNOF)
#define CAPTURE_DESCRIPTOR_ALIGN	CAMRTC_ALIGN(CAPTURE_DESCRIPTOR_ALIGNOF)

typedef uint64_t iova_t CAPTURE_IVC_ALIGN;

#define SYNCPOINT_ID_INVALID	MK_U32(0)
#define GOS_INDEX_INVALID	MK_U8(0xFF)

#pragma GCC diagnostic warning "-Wdeprecated-declarations"
#define CAMRTC_DEPRECATED __attribute__((deprecated))

/*Status Fence Support*/
#define STATUS_FENCE_SUPPORT

typedef struct syncpoint_info {
	/** Syncpoint ID */
	uint32_t id;
	/** Syncpoint threshold when storing a fence [0, UIN32_MAX] */
	uint32_t threshold;
	/** Grid-of-Semaphores (GoS) SMMU stream id [1, 127] (non-safety) */
	uint8_t gos_sid;
	/**
	 * Index into a table of GoS page base pointers (see @ref
	 * capture_channel_config::vi_gos_tables) (non-safety)
	 */
	uint8_t gos_index;
	/** Offset of a semaphore within a Grid-of-Semaphores [0, 63] (non-safety) */
	uint16_t gos_offset;
	/** Reserved */
	uint32_t pad_;
	/** IOVA address of the Host1x syncpoint register. Must be a multiple of 4. */
	iova_t shim_addr;
} syncpoint_info_t CAPTURE_IVC_ALIGN;

/**
 * @defgroup StatsSize Statistics data size defines for ISP5
 *
 * The size for each unit includes the standard ISP5 HW stats
 * header size.
 *
 * Size break down for each unit.
 *  FB = 32 byte header + (256 x 4) bytes. FB has 256 windows with 4 bytes
 *       of stats data per window.
 *  FM = 32 byte header + (64 x 64 x 2 x 4) bytes. FM can have 64 x 64 windows
 *       with each window having 2 bytes of data for each color channel.
 *  AFM = 32 byte header + 8 byte statistics data + 8 bytes padding per ROI.
 *  LAC = 32 byte header + ( (32 x 32) x ((4 + 2 + 2) x 4) )
 *        Each ROI has 32x32 windows with each window containing 8
 *        bytes of data per color channel.
 *  Hist = Header + (256 x 4 x 4) bytes since Hist unit has 256 bins and
 *         each bin collects 4 byte data for each color channel + 4 Dwords for
 *         excluded pixel count due to elliptical mask per color channel.
 *  Pru = 32 byte header + (8 x 4) bytes for bad pixel count and accumulated
 *        pixel adjustment for pixels both inside and outside the ROI.
 *  LTM = 32 byte header + (128 x 4) bytes for histogram data + (8 x 8 x 4 x 2)
 *        bytes for soft key average and count. Soft key statistics are
 *        collected by dividing the frame into a 8x8 array region.
 * @{
 */
/** Statistics unit hardware header size in bytes */
#define ISP5_STATS_HW_HEADER_SIZE	MK_SIZE(32)
/** Flicker band (FB) unit statistics data size in bytes */
#define ISP5_STATS_FB_MAX_SIZE		MK_SIZE(1056)
/** Focus Metrics (FM) unit statistics data size in bytes */
#define ISP5_STATS_FM_MAX_SIZE		MK_SIZE(32800)
/** Auto Focus Metrics (AFM) unit statistics data size in bytes */
#define ISP5_STATS_AFM_ROI_MAX_SIZE	MK_SIZE(48)
/** Local Average Clipping (LAC) unit statistics data size in bytes */
#define ISP5_STATS_LAC_ROI_MAX_SIZE	MK_SIZE(32800)
/** Histogram unit statistics data size in bytes */
#define ISP5_STATS_HIST_MAX_SIZE	MK_SIZE(4144)
/** Pixel Replacement Unit (PRU) unit statistics data size in bytes */
#define ISP5_STATS_OR_MAX_SIZE		MK_SIZE(64)
/** Local Tone Mapping (LTM) unit statistics data size in bytes */
#define ISP5_STATS_LTM_MAX_SIZE		MK_SIZE(1056)

/* Stats buffer addresses must be aligned to 64 byte (ATOM) boundaries */
#define ISP5_ALIGN_STAT_OFFSET(_offset) \
	(((uint32_t)(_offset) + MK_U32(63)) & ~(MK_U32(63)))

/** Flicker band (FB) unit statistics data offset */
#define ISP5_STATS_FB_OFFSET		MK_SIZE(0)
/** Focus Metrics (FM) unit statistics data offset */
#define ISP5_STATS_FM_OFFSET \
	(ISP5_STATS_FB_OFFSET + ISP5_ALIGN_STAT_OFFSET(ISP5_STATS_FB_MAX_SIZE))
/** Auto Focus Metrics (AFM) unit statistics data offset */
#define ISP5_STATS_AFM_OFFSET \
	(ISP5_STATS_FM_OFFSET + ISP5_ALIGN_STAT_OFFSET(ISP5_STATS_FM_MAX_SIZE))
/** Local Average Clipping (LAC0) unit statistics data offset */
#define ISP5_STATS_LAC0_OFFSET \
	(ISP5_STATS_AFM_OFFSET + \
	(ISP5_ALIGN_STAT_OFFSET(ISP5_STATS_AFM_ROI_MAX_SIZE) * MK_SIZE(8)))
/** Local Average Clipping (LAC1) unit statistics data offset */
#define ISP5_STATS_LAC1_OFFSET \
	(ISP5_STATS_LAC0_OFFSET + \
	(ISP5_ALIGN_STAT_OFFSET(ISP5_STATS_LAC_ROI_MAX_SIZE) * MK_SIZE(4)))
/** Histogram unit (H0) statistics data offset */
#define ISP5_STATS_HIST0_OFFSET \
	(ISP5_STATS_LAC1_OFFSET + \
	(ISP5_ALIGN_STAT_OFFSET(ISP5_STATS_LAC_ROI_MAX_SIZE) * MK_SIZE(4)))
/** Histogram unit (H1) statistics data offset */
#define ISP5_STATS_HIST1_OFFSET \
	(ISP5_STATS_HIST0_OFFSET + \
	ISP5_ALIGN_STAT_OFFSET(ISP5_STATS_HIST_MAX_SIZE))
/** Pixel Replacement Unit (PRU) unit statistics data offset */
#define ISP5_STATS_OR_OFFSET \
	(ISP5_STATS_HIST1_OFFSET + \
	ISP5_ALIGN_STAT_OFFSET(ISP5_STATS_HIST_MAX_SIZE))
/** Local Tone Mapping (LTM) unit statistics data offset */
#define ISP5_STATS_LTM_OFFSET \
	(ISP5_STATS_OR_OFFSET + \
	ISP5_ALIGN_STAT_OFFSET(ISP5_STATS_OR_MAX_SIZE))
/** Total statistics data size in bytes */
#define ISP5_STATS_TOTAL_SIZE \
	(ISP5_STATS_LTM_OFFSET + ISP5_STATS_LTM_MAX_SIZE)
/**@}*/

/**
 * @defgroup StatsSize Statistics data size defines for ISP6
 *
 * The size for each unit includes the standard ISP6 HW stats
 * header size.
 *
 * Size break down for each unit.
 *  FB = 32 byte header + (512 x 4) bytes. FB has 512 windows with 4 bytes
 *       of stats data per window.
 *  FM = 32 byte header + (64 x 64 x 2 x 4) bytes. FM can have 64 x 64 windows
 *       with each window having 2 bytes of data for each color channel.
 *  AFM = 32 byte header + 8 byte statistics data + 8 byte pad per ROI.
 *  LAC = 32 byte header + ( (32 x 32) x ((4 + 2 + 2) x 4) )
 *        Each ROI has 32x32 windows with each window containing 8
 *        bytes of data per color channel.
 *  Hist = Header + (256 x 4 x 4) bytes since Hist unit has 256 bins and
 *         each bin collects 4 byte data for each color channel + 4 Dwords for
 *         excluded pixel count due to elliptical mask per color channel.
 *  OR  = 32 byte header + (8 x 4) bytes for bad pixel count and accumulated
 *        pixel adjustment for pixels both inside and outside the ROI.
 *  PRU hist = Header + (256 x 4 x 4) bytes since Hist unit has 256 bins and
 *         each bin collects 4 byte data for each color channel + 4 Dwords for
 *         excluded pixel count due to elliptical mask per color channel.
 *  LTM = 32 byte header + (128 x 4) bytes for histogram data + (8 x 8 x 4 x 2)
 *        bytes for soft key average and count. Soft key statistics are
 *        collected by dividing the frame into a 8x8 array region.
 * @{
 */
/** Statistics unit hardware header size in bytes */
#define ISP6_STATS_HW_HEADER_SIZE	MK_SIZE(32)
/** Flicker band (FB) unit statistics data size in bytes */
#define ISP6_STATS_FB_MAX_SIZE		MK_SIZE(2080)
/** Focus Metrics (FM) unit statistics data size in bytes */
#define ISP6_STATS_FM_MAX_SIZE		MK_SIZE(32800)
/** Auto Focus Metrics (AFM) unit statistics data size in bytes */
#define ISP6_STATS_AFM_ROI_MAX_SIZE	MK_SIZE(48)
/** Local Average Clipping (LAC) unit statistics data size in bytes */
#define ISP6_STATS_LAC_ROI_MAX_SIZE	MK_SIZE(32800)
/** Histogram unit statistics data size in bytes */
#define ISP6_STATS_HIST_MAX_SIZE	MK_SIZE(4144)
/** Pixel Replacement Unit (PRU) unit statistics data size in bytes */
#define ISP6_STATS_OR_MAX_SIZE		MK_SIZE(64)
/** PRU histogram (HIST_RAW24) unit statistics data size in bytes */
#define ISP6_STATS_HIST_RAW24_MAX_SIZE	MK_SIZE(1056)
/** Local Tone Mapping (LTM) unit statistics data size in bytes */
#define ISP6_STATS_LTM_MAX_SIZE		MK_SIZE(1056)
/* Stats buffer addresses muse be aligned to 64 byte (ATOM) boundaries */
#define ISP6_ALIGN_STAT_OFFSET(_offset) \
	(((uint32_t)(_offset) + MK_U32(63)) & ~(MK_U32(63)))

/** Flicker band (FB) unit statistics data offset */
#define ISP6_STATS_FB_OFFSET		MK_SIZE(0)
/** Focus Metrics (FM) unit statistics data offset */
#define ISP6_STATS_FM_OFFSET \
	(ISP6_STATS_FB_OFFSET + ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_FB_MAX_SIZE))
/** Auto Focus Metrics (AFM) unit statistics data offset */
#define ISP6_STATS_AFM_OFFSET \
	(ISP6_STATS_FM_OFFSET + ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_FM_MAX_SIZE))
/** Local Average Clipping (LAC0) unit statistics data offset */
#define ISP6_STATS_LAC0_OFFSET \
	(ISP6_STATS_AFM_OFFSET + \
	(ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_AFM_ROI_MAX_SIZE) * MK_SIZE(8)))
/** Local Average Clipping (LAC1) unit statistics data offset */
#define ISP6_STATS_LAC1_OFFSET \
	(ISP6_STATS_LAC0_OFFSET + \
	(ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_LAC_ROI_MAX_SIZE) * MK_SIZE(4)))
/** Histogram unit (H0) statistics data offset */
#define ISP6_STATS_HIST0_OFFSET \
	(ISP6_STATS_LAC1_OFFSET + \
	(ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_LAC_ROI_MAX_SIZE) * MK_SIZE(4)))
/** Histogram unit (H1) statistics data offset */
#define ISP6_STATS_HIST1_OFFSET \
	(ISP6_STATS_HIST0_OFFSET + \
	ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_HIST_MAX_SIZE))
/** Outlier replacement (OR) unit statistics data offset */
#define ISP6_STATS_OR_OFFSET \
	(ISP6_STATS_HIST1_OFFSET + \
	ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_HIST_MAX_SIZE))
/** Raw data 24 bit histogram (HIST_RAW24) unit statistics data offset */
#define ISP6_STATS_HIST_RAW24_OFFSET \
	(ISP6_STATS_OR_OFFSET + \
	ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_OR_MAX_SIZE))
/** Local Tone Mapping (LTM) unit statistics data offset */
#define ISP6_STATS_LTM_OFFSET \
	(ISP6_STATS_HIST_RAW24_OFFSET + \
	ISP6_ALIGN_STAT_OFFSET(ISP6_STATS_HIST_RAW24_MAX_SIZE))
/** Total statistics data size in bytes */
#define ISP6_STATS_TOTAL_SIZE \
	(ISP6_STATS_LTM_OFFSET + ISP6_STATS_LTM_MAX_SIZE)
/**@}*/

/**
 * @defgroup StatsSize Statistics data size defines for ISP7
 *
 * The size for each unit includes the standard ISP7 HW stats
 * header size.
 *
 * Size break down for each unit.
 *  FB = 32 byte header + (512 x 4) bytes. FB has 512 windows with 4 bytes
 *       of stats data per window.
 *  FM = 32 byte header + (64 x 64 x 2 x 4) bytes. FM can have 64 x 64 windows
 *       with each window having 2 bytes of data for each color channel.
 *  AFM = 32 byte header + 8 byte statistics data + 8 byte padding per ROI.
 *  LAC = 32 byte header + ( (32 x 32) x ((4 + 2 + 2) x 4) )
 *        Each ROI has 32x32 windows with each window containing 8
 *        bytes of data per color channel.
 *  Hist = Header + (256 x 4 x 4) bytes since Hist unit has 256 bins and
 *         each bin collects 4 byte data for each color channel + 4 Dwords for
 *         excluded pixel count due to elliptical mask per color channel.
 *  DPC  = 32 byte header + (24 x 4) bytes for bad pixel count and accumulated
 *        pixel adjustment for pixels both inside and outside the ROI.
 *  LTM = 32 byte header + (128 x 4) bytes for histogram data + (8 x 8 x 4 x 2)
 *        bytes for soft key average and count. Soft key statistics are
 *        collected by dividing the frame into a 8x8 array region.
 * @{
 */
/** Statistics unit hardware header size in bytes */
#define ISP7_STATS_HW_HEADER_SIZE	MK_SIZE(32)
/** Flicker band (FB) unit statistics data size in bytes */
#define ISP7_STATS_FB_MAX_SIZE		MK_SIZE(2080)
/** Focus Metrics (FM) unit statistics data size in bytes */
#define ISP7_STATS_FM_MAX_SIZE		MK_SIZE(32800)
/** Auto Focus Metrics (AFM) unit statistics data size in bytes */
#define ISP7_STATS_AFM_ROI_MAX_SIZE	MK_SIZE(48)
/** Local Average Clipping (LAC) unit statistics data size in bytes */
#define ISP7_STATS_LAC_ROI_MAX_SIZE	MK_SIZE(32800)
/** Histogram unit statistics data size in bytes */
#define ISP7_STATS_HIST_MAX_SIZE	MK_SIZE(4144)
/** Pixel Replacement Unit (PRU) unit statistics data size in bytes */
#define ISP7_STATS_DPC_MAX_SIZE		MK_SIZE(128)
/** Local Tone Mapping (LTM) unit statistics data size in bytes */
#define ISP7_STATS_LTM_MAX_SIZE		MK_SIZE(1056)
/* Stats buffer addresses must be aligned to 64 byte (ATOM) boundaries */
#define ISP7_ALIGN_STAT_OFFSET(_offset) \
	(((uint32_t)(_offset) + MK_U32(63)) & ~(MK_U32(63)))

/** Flicker band (FB) unit statistics data offset */
#define ISP7_STATS_FB_OFFSET		MK_SIZE(0)
/** Focus Metrics (FM) unit statistics data offset */
#define ISP7_STATS_FM_OFFSET \
	(ISP7_STATS_FB_OFFSET + ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_FB_MAX_SIZE))
/** Auto Focus Metrics (AFM) unit statistics data offset */
#define ISP7_STATS_AFM_OFFSET \
	(ISP7_STATS_FM_OFFSET + ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_FM_MAX_SIZE))
/** Local Average Clipping (LAC0) unit statistics data offset */
#define ISP7_STATS_LAC0_OFFSET \
	(ISP7_STATS_AFM_OFFSET + \
	(ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_AFM_ROI_MAX_SIZE) * MK_SIZE(8)))
/** Local Average Clipping (LAC1) unit statistics data offset */
#define ISP7_STATS_LAC1_OFFSET \
	(ISP7_STATS_LAC0_OFFSET + \
	(ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_LAC_ROI_MAX_SIZE) * MK_SIZE(4)))
/** Histogram unit (H0) statistics data offset */
#define ISP7_STATS_HIST0_OFFSET \
	(ISP7_STATS_LAC1_OFFSET + \
	(ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_LAC_ROI_MAX_SIZE) * MK_SIZE(4)))
/** Histogram unit (H1) statistics data offset */
#define ISP7_STATS_HIST1_OFFSET \
	(ISP7_STATS_HIST0_OFFSET + \
	ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_HIST_MAX_SIZE))
/** Histogram unit (H2) statistics data offset */
#define ISP7_STATS_HIST2_OFFSET \
	(ISP7_STATS_HIST1_OFFSET + \
	ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_HIST_MAX_SIZE))
/** Outlier replacement (OR) unit statistics data offset */
#define ISP7_STATS_DPC_OFFSET \
	(ISP7_STATS_HIST2_OFFSET + \
	ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_HIST_MAX_SIZE))
/** Local Tone Mapping (LTM) unit statistics data offset */
#define ISP7_STATS_LTM_OFFSET \
	(ISP7_STATS_DPC_OFFSET + \
	ISP7_ALIGN_STAT_OFFSET(ISP7_STATS_DPC_MAX_SIZE))
/** Total statistics data size in bytes */
#define ISP7_STATS_TOTAL_SIZE \
	(ISP7_STATS_LTM_OFFSET + ISP7_STATS_LTM_MAX_SIZE)
/**@}*/

#define ISP_NUM_GOS_TABLES	MK_U32(8)

/**
 * @defgroup ISPUnitIds
 * ISP Unit Identifiers
 */
/**@{*/
/** ISP unit 0 */
#define ISP_UNIT_ISP                              MK_U32(0x0000)
/** ISP unit 1 */
#define ISP_UNIT_ISP2                             MK_U32(0x0001)
/**@}*/

#define VI_NUM_GOS_TABLES	MK_U32(12)
#define VI_NUM_ATOMP_SURFACES	4U
#define VI_NUM_STATUS_SURFACES	1U
#define VI_NUM_VI_PFSD_SURFACES	2U

/**
 * @defgroup ViAtompSurface VI ATOMP surface related defines
 * @{
 */
/** Output surface plane 0 */
#define VI_ATOMP_SURFACE0				0U
/** Output surface plane 1 */
#define VI_ATOMP_SURFACE1				1U
/** Output surface plane 2 */
#define VI_ATOMP_SURFACE2				2U

/** Sensor embedded data */
#define VI_ATOMP_SURFACE_EMBEDDED 3

/** RAW pixels */
#define VI_ATOMP_SURFACE_MAIN				VI_ATOMP_SURFACE0
/** PDAF pixels */
#define VI_ATOMP_SURFACE_PDAF				VI_ATOMP_SURFACE1

/** YUV - Luma plane */
#define VI_ATOMP_SURFACE_Y				VI_ATOMP_SURFACE0
/** Semi-planar - UV plane */
#define	VI_ATOMP_SURFACE_UV				VI_ATOMP_SURFACE1
/** Planar - U plane */
#define	VI_ATOMP_SURFACE_U				VI_ATOMP_SURFACE1
/** Planar - V plane */
#define	VI_ATOMP_SURFACE_V				VI_ATOMP_SURFACE2

/** @} */

/* SLVS-EC */
#define SLVSEC_STREAM_DISABLED	MK_U8(0xFF)

/**
 * @defgroup VICaptureChannelFlags VI Capture channel specific flags
 */
/**@{*/
/** Channel takes input from Video Interface (VI) */
#define CAPTURE_CHANNEL_FLAG_VIDEO			MK_U32(0x0001)

/** Channel supports RAW Bayer output */
#define CAPTURE_CHANNEL_FLAG_RAW			MK_U32(0x0002)

/** Channel supports planar YUV output */
#define CAPTURE_CHANNEL_FLAG_PLANAR			MK_U32(0x0004)

/** Channel supports semi-planar YUV output */
#define CAPTURE_CHANNEL_FLAG_SEMI_PLANAR		MK_U32(0x0008)

/** Channel supports phase-detection auto-focus (non-safety) */
#define CAPTURE_CHANNEL_FLAG_PDAF			MK_U32(0x0010)

/** Channel outputs sensor embedded data */
#define CAPTURE_CHANNEL_FLAG_EMBDATA			MK_U32(0x0040)

/** Channel outputs to ISPA (deprecated, non-safety) */
#define CAPTURE_CHANNEL_FLAG_ISPA			MK_U32(0x0080)

/** Channel outputs to ISPB (deprecated, non-safety) */
#define CAPTURE_CHANNEL_FLAG_ISPB			MK_U32(0x0100)

/**
 * Channel outputs directly to selected ISP (ISO mode)
 * (deprecated, non-safety)
 */
#define CAPTURE_CHANNEL_FLAG_ISP_DIRECT			MK_U32(0x0200)

/** Channel outputs to software ISP (reserved) */
#define CAPTURE_CHANNEL_FLAG_ISPSW			MK_U32(0x0400)
/**
 * Channel treats all errors as stop-on-error and requires reset
 * for recovery (non-safety)
 */
#define CAPTURE_CHANNEL_FLAG_RESET_ON_ERROR		MK_U32(0x0800)

/** Channel has line timer enabled */
#define CAPTURE_CHANNEL_FLAG_LINETIMER			MK_U32(0x1000)

/** Channel supports SLVSEC sensors (non-safety) */
#define CAPTURE_CHANNEL_FLAG_SLVSEC			MK_U32(0x2000)

/**
 * Channel reports errors to System Error Handler based on
 * @ref capture_channel_config::error_mask_correctable and
 * @ref capture_channel_config::error_mask_uncorrectable.
 */
#define CAPTURE_CHANNEL_FLAG_ENABLE_HSM_ERROR_MASKS	MK_U32(0x4000)

/**
 * Enable Permanent Fault Software diagnostics (PFSD) for this VI channel.
 * VI will insert a test pattern into a specified part of the frame, to
 * be checked after frame capture. A correct test pattern will verify
 * that VI internal pipeline is functioning normally.
 */
#define CAPTURE_CHANNEL_FLAG_ENABLE_VI_PFSD		MK_U32(0x8000)

/** Channel binds to a CSI stream and channel */
#define CAPTURE_CHANNEL_FLAG_CSI			MK_U32(0x10000)
/**@}*/

/**
 * @defgroup CaptureChannelErrMask VI error numbers
 */
/**@{*/

/** VI Frame start error timeout */
#define CAPTURE_CHANNEL_ERROR_VI_FRAME_START_TIMEOUT	MK_BIT32(23)

/** VI Permanent Fault SW Diagnostics (PFSD) error */
#define CAPTURE_CHANNEL_ERROR_VI_PFSD_FAULT		MK_BIT32(22)

/** Embedded data incomplete */
#define CAPTURE_CHANNEL_ERROR_ERROR_EMBED_INCOMPLETE	MK_BIT32(21)

/** Pixel frame is incomplete */
#define CAPTURE_CHANNEL_ERROR_INCOMPLETE		MK_BIT32(20)
/**
 * A Frame End appears from NVCSI before the normal number of pixels
 * has appeared
 */
#define CAPTURE_CHANNEL_ERROR_STALE_FRAME		MK_BIT32(19)

/** A start-of-frame matches a channel that is already in frame */
#define CAPTURE_CHANNEL_ERROR_COLLISION			MK_BIT32(18)

/** Frame end was forced by channel reset */
#define CAPTURE_CHANNEL_ERROR_FORCE_FE			MK_BIT32(17)

/**
 * A LOAD command is received for a channel while that channel is
 * currently in a frame.
 */
#define CAPTURE_CHANNEL_ERROR_LOAD_FRAMED		MK_BIT32(16)

/** The pixel datatype changed in the middle of the line */
#define CAPTURE_CHANNEL_ERROR_DTYPE_MISMATCH		MK_BIT32(15)

/** Unexpected embedded data in frame */
#define CAPTURE_CHANNEL_ERROR_EMBED_INFRINGE		MK_BIT32(14)

/** Extra embedded bytes on line */
#define CAPTURE_CHANNEL_ERROR_EMBED_LONG_LINE		MK_BIT32(13)

/** Embedded bytes found between line start and line end*/
#define CAPTURE_CHANNEL_ERROR_EMBED_SPURIOUS		MK_BIT32(12)

/** Too many embeded lines in frame */
#define CAPTURE_CHANNEL_ERROR_EMBED_RUNAWAY		MK_BIT32(11)

/** Two embedded line starts without a line end in between */
#define CAPTURE_CHANNEL_ERROR_EMBED_MISSING_LE		MK_BIT32(10)

/** A line has fewer pixels than expected width */
#define CAPTURE_CHANNEL_ERROR_PIXEL_SHORT_LINE		MK_BIT32(9)

/** A line has more pixels than expected width, pixels dropped */
#define CAPTURE_CHANNEL_ERROR_PIXEL_LONG_LINE		MK_BIT32(8)

/** A pixel found between line end and line start markers, dropped */
#define CAPTURE_CHANNEL_ERROR_PIXEL_SPURIOUS		MK_BIT32(7)

/** Too many pixel lines in frame, extra lines dropped */
#define CAPTURE_CHANNEL_ERROR_PIXEL_RUNAWAY		MK_BIT32(6)

/** Two lines starts without a line end in between */
#define CAPTURE_CHANNEL_ERROR_PIXEL_MISSING_LE		MK_BIT32(5)

/**@}*/

/**
 * @defgroup VIUnitIds VI Unit Identifiers
 */
/**@{*/
/** VI unit 0 */
#define VI_UNIT_VI					MK_U32(0x0000)
/** VI unit 1 */
#define VI_UNIT_VI2					MK_U32(0x0001)
/**@}*/

/**
 * @brief Identifies a specific CSI stream.
 */
struct csi_stream_config {
	/** See @ref NvCsiStream "NvCSI Stream ID" */
	uint32_t stream_id;
	/**
	 * See @ref NvCsiPort "CSI Port". If the CSI port is specified,
	 * the value must map correctly to the @ref stream_id value.
	 */
	uint32_t csi_port;
	/** See @ref NvCsiVirtualChannel "CSI Virtual Channel". */
	uint32_t virtual_channel;
	/** Reserved */
	uint32_t pad__;
};

/**
 * @brief Describes RTCPU side resources for a capture pipe-line.
 */
struct capture_channel_config {
	/**
	 * A bitmask describing the set of non-shareable
	 * HW resources that the capture channel will need. These HW resources
	 * will be assigned to the new capture channel and will be owned by the
	 * channel until it is released with @ref CAPTURE_CHANNEL_RELEASE_REQ.
	 *
	 * VI channels can have different capabilities. The flags are checked
	 * against the VI channel capabilities to make sure the allocated VI
	 * channel meets the requirements.
	 *
	 * See @ref VICaptureChannelFlags "Capture Channel Flags".
	 */
	uint32_t channel_flags;

	/** rtcpu internal data field - Should be set to zero */
	uint32_t channel_id;

	/** VI unit ID. See @ref ViUnitIds "VI Unit Identifiers" */
	uint32_t vi_unit_id;

	/** Reserved */
	uint32_t pad__;

	/**
	 * A bitmask indicating which VI hardware channels to consider for
	 * allocation [0, 0xFFFFFFFFF]. LSB is VI channel 0.
	 *
	 * Normal usage is to set all the bits, so as not to restrict the
	 * channel allocation. Note that a client VM may also have additional
	 * restrictions on the range of VI channels available to it.
	 *
	 * This control is provided for special use cases and for testing.
	 */
	uint64_t vi_channel_mask;

	/**
	 * A bitmask indicating which VI2 hardware channels to consider for
	 * allocation [0, 0xFFFFFFFFF]. LSB is VI2 channel 0.
	 *
	 * Normal usage is to set all the bits, so as not to restrict the
	 * channel allocation. Note that a client VM may also have additional
	 * restrictions on the range of VI channels available to it.
	 *
	 * This control is provided for special use cases and for testing.
	 */
	uint64_t vi2_channel_mask;

	/**
	 * CSI stream configuration identifies the CSI stream input for
	 * this channel.
	 */
	struct csi_stream_config csi_stream;

	/**
	 * Base address of the @ref capture_descriptor ring buffer.
	 * The size of the buffer is @ref queue_depth * @ref request_size.
	 * The value must be non-zero and a multiple of
	 * @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	iova_t requests;

	/**
	 * Base address of a memory mapped ring buffer containing capture
	 * requests buffer information. The size of the buffer is @ref
	 * queue_depth * @ref request_memoryinfo_size. The value must be
	 * non-zero and a multiple of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	iova_t requests_memoryinfo;

	/**
	 * Maximum number of capture requests in the requests queue [1, 240].
	 * Determines the size of the @ref capture_descriptor ring buffer
	 * (@ref requests).
	 */
	uint32_t queue_depth;

	/**
	 * Size of the buffer reserved for each capture descriptor. The size
	 * must be >= sizeof(@ref capture_descriptor) and a multiple of
	 * @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	uint32_t request_size;

	/**
	 * Size of the memoryinfo buffer reserved for each capture request.
	 * Must be >= sizeof(@ref capture_descriptor_memoryinfo) and
	 * a multiple of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	uint32_t request_memoryinfo_size;

	/** Reserved */
	uint32_t reserved2;

	/** SLVS-EC main stream (non-safety) */
	uint8_t slvsec_stream_main;
	/** SLVS-EC sub stream (non-safety) */
	uint8_t slvsec_stream_sub;
	/** Reserved */
	uint16_t reserved1;

#define HAVE_VI_GOS_TABLES
	/**
	 * Number of elements in @ref vi_gos_tables array
	 * [0, @ref VI_NUM_GOS_TABLES].
	 */
	uint32_t num_vi_gos_tables;
	/**
	 * Array of IOVA pointers to VI Grid-of-Semaphores (GoS) tables
	 * (non-safety).
	 *
	 * GoS table configuration, if present, must be the same on all
	 * active channels. The IOVA addresses must be a multiple of 256.
	 */
	iova_t vi_gos_tables[VI_NUM_GOS_TABLES];
	/**
	 * Capture progress syncpoint information. The progress syncpoint
	 * is incremented on Start-Of-Frame, whenever a slice of pixel
	 * data has been written to memory, and finally when the status
	 * of a capture has been written to memory. The same progress
	 * syncpoint will keep incrementing for every consecutive capture
	 * request.
	 *
	 * @ref syncpoint_info::threshold must be set to the initial
	 * value of the hardware syncpoint on channel setup.
	 **/
	struct syncpoint_info progress_sp;
	/**
	 * Embedded data syncpoint information. The embedded data syncpoint
	 * is incremented whenever the sensor embedded data for a captured
	 * frame has been written to memory. The embedded data syncpoint
	 * is optional and need not be specified if embedded data is not
	 * being captured (see @ref vi_channel_config::embdata_enable).
	 * The same embedded data syncpoint will keep incrementing for every
	 * consecutive capture request.
	 *
	 * @ref syncpoint_info::threshold must be set to the initial
	 * value of the hardware syncpoint on channel setup.
	 **/
	struct syncpoint_info embdata_sp;
	/**
	 * VI line timer syncpoint info. The line timer syncpoint is
	 * incremented whenever the frame readout reaches a specified
	 * line (see @ref vi_channel_config::line_timer,
	 * @ref vi_channel_config::line_timer_first, and
	 * @ref vi_channel_config::line_timer_periodic). The line timer
	 * syncpoint is optional and need not be specified if line timer
	 * is not enabled (see @ref vi_channel_config::line_timer_enable).
	 * The same line timer syncpoint will keep incrementing for every
	 * consecutive capture request.
	 *
	 * @ref syncpoint_info::threshold must be set to the initial
	 * value of the hardware syncpoint on channel setup.
	 **/
	struct syncpoint_info linetimer_sp;
	/**
	 * Error mask for suppressing uncorrected safety errors (see @ref
	 * CaptureChannelErrMask "VI error numbers"). If the mask is set to
	 * zero, all VI errors will be reported as uncorrected safety errors.
	 * If a specific error is masked by setting the corresponding bit
	 * (1 << <em>error number</em>) in the error mask, the error will not
	 * be reported as uncorrected. Note that the error may still be
	 * reported as a corrected error unless it is also masked in
	 * @ref error_mask_correctable.
	 *
	 * An uncorrected error will be interpreted by System Error Handler
	 * as an indication that a camera is off-line and cannot continue
	 * the capture.
	 *
	 * @ref CAPTURE_CHANNEL_FLAG_ENABLE_HSM_ERROR_MASKS must be set in
	 * @ref channel_flags for this setting to take effect. Otherwise,
	 * a default error mask will be used.
	 */
	uint32_t error_mask_uncorrectable;
	/**
	 * This error mask applies only to errors that are masked in @ref
	 * error_mask_uncorrectable. By default, all such errors are reported
	 * to System Error Handler as corrected. If a specific error is masked
	 * in both this mask and in @ref error_mask_uncorrectable, by setting
	 * the corresponding bit (1 << <em>error number</em>) in the error mask
	 * (see @ref CaptureChannelErrMask "VI error numbers"), the error
	 * will not be reported to System Error Handler at all.
	 *
	 * A corrected safety error will be interpreted by System Error Handler
	 * as an indication that a single frame has been corrupted or dropped.
	 *
	 * @ref CAPTURE_CHANNEL_FLAG_ENABLE_HSM_ERROR_MASKS must be set in
	 * @ref channel_flags for this setting to take effect. Otherwise,
	 * a default error mask will be used.
	 */
	uint32_t error_mask_correctable;
	/**
	 * When a capture error is detected, the capture channel will
	 * enter an error state if the corresponding error bit
	 * (1 << <em>error number</em>) is set in this bit mask
	 * (see @ref ViNotifyErrorTag "VI errors" for the bit definitions).
	 *
	 * When the channel is in error state it will not accept any new
	 * capture requests.
	 */
	uint64_t stop_on_error_notify_bits;

} CAPTURE_IVC_ALIGN;

/**
 * @defgroup ViDpcmModes VI DPCM Modes (non-safety)
 */
/**@{*/

/**  T186-style RAW10 format */
#define VI_DPCM_RAW10				MK_U8(0)

/**  T186-style RAW12 format */
#define VI_DPCM_RAW12				MK_U8(1)

/** Run-length-encoded RAW10 format */
#define VI_DPCM_RLE_RAW10			MK_U8(2)

/** Run-length-encoded RAW12 format */
#define VI_DPCM_RLE_RAW12			MK_U8(3)

/** RAW16 format for logarithmic data */
#define VI_DPCM_RAW16				MK_U8(4)

/** RAW20 format */
#define VI_DPCM_RAW20				MK_U8(5)

/**@}*/

/**
 * @defgroup ViPixFmt VI Output Pixel Formats
 */
/**@{*/
/** NvColorFormat_R5G6B5 */
#define VI_PIXFMT_FORMAT_T_R5G6B5		MK_U8(1)

/** NvColorFormat_B5G6R5 */
#define VI_PIXFMT_FORMAT_T_B5G6R5		MK_U8(2)

/** NvColorFormat_R8 */
#define VI_PIXFMT_FORMAT_T_R8			MK_U8(5)

/** NvColorFormat_A8B8G8R8 */
#define VI_PIXFMT_FORMAT_T_A8B8G8R8		MK_U8(8)

/** NvColorFormat_A8R8G8B8 */
#define VI_PIXFMT_FORMAT_T_A8R8G8B8		MK_U8(9)

/** NvColorFormat_B8G8R8A8 */
#define VI_PIXFMT_FORMAT_T_B8G8R8A8		MK_U8(10)

/** NvColorFormat_R8G8B8A8 */
#define VI_PIXFMT_FORMAT_T_R8G8B8A8		MK_U8(11)

/** NvColorFormat_Y8_U8__Y8_V8 or NvColorFormat_YUYV */
#define VI_PIXFMT_FORMAT_T_Y8_U8__Y8_V8		MK_U8(16)

/** NvColorFormat_Y8_V8__Y8_U8 or NvColorFormat_YVYU */
#define VI_PIXFMT_FORMAT_T_Y8_V8__Y8_U8		MK_U8(17)

/** NvColorFormat_V8_Y8__U8_Y8 or NvColorFormat_VYUY */
#define VI_PIXFMT_FORMAT_T_V8_Y8__U8_Y8		MK_U8(18)

/** NvColorFormat_U8_Y8__V8_Y8 or NvColorFormat_UYVY */
#define VI_PIXFMT_FORMAT_T_U8_Y8__V8_Y8		MK_U8(19)

/** NV21 format (YUV420 semi-planar: Y8,U8V8) */
#define VI_PIXFMT_FORMAT_T_Y8__U8V8_N420	MK_U8(34)

/** NV12 format (YUV420 semi-planar: Y8,V8U8) */
#define VI_PIXFMT_FORMAT_T_Y8__V8U8_N420	MK_U8(35)

/** NvColorFormat_B5G5R5A1 */
#define VI_PIXFMT_FORMAT_T_B5G5R5A1		MK_U8(42)

/** NvColorFormat_R5G5B5A1 */
#define VI_PIXFMT_FORMAT_T_R5G5B5A1		MK_U8(43)

/** NV61 format (YUV422 semi-planar: Y8,U8V8) */
#define VI_PIXFMT_FORMAT_T_Y8__U8V8_N422	MK_U8(44)

/** NV16 format (YUV422 semi-planar: Y8,V8U8) */
#define VI_PIXFMT_FORMAT_T_Y8__V8U8_N422	MK_U8(45)

/** YV16 format (YUV422 full planar: Y8,U8,V8) */
#define VI_PIXFMT_FORMAT_T_Y8__U8__V8_N422	MK_U8(46)

/** YV12 format (YUV420 full planar: Y8,U8,V8) */
#define VI_PIXFMT_FORMAT_T_Y8__U8__V8_N420	MK_U8(47)

/** RLE compressed format for RAW10 data */
#define VI_PIXFMT_FORMAT_T_DPCM_RAW10		MK_U8(64)

/** NvColorFormat_A2B10G10R10 */
#define VI_PIXFMT_FORMAT_T_A2B10G10R10		MK_U8(68)

/** NvColorFormat_A2R10G10B10 */
#define VI_PIXFMT_FORMAT_T_A2R10G10B10		MK_U8(69)

/** NvColorFormat_B10G10R10A2 */
#define VI_PIXFMT_FORMAT_T_B10G10R10A2		MK_U8(70)

/** NvColorFormat_R10G10B10A2 */
#define VI_PIXFMT_FORMAT_T_R10G10B10A2		MK_U8(71)

/** NvColorFormat_A4B4G4R4 */
#define VI_PIXFMT_FORMAT_T_A4B4G4R4		MK_U8(80)

/** NvColorFormat_A4R4G4B4 */
#define VI_PIXFMT_FORMAT_T_A4R4G4B4		MK_U8(81)

/** NvColorFormat_B4G4R4A4 */
#define VI_PIXFMT_FORMAT_T_B4G4R4A4		MK_U8(82)

/** NvColorFormat_R4G4B4A4 */
#define VI_PIXFMT_FORMAT_T_R4G4B4A4		MK_U8(83)

/** NvColorFormat_A1B5G5R5 */
#define VI_PIXFMT_FORMAT_T_A1B5G5R5		MK_U8(84)

/** NvColorFormat_A1R5G5B5 */
#define VI_PIXFMT_FORMAT_T_A1R5G5B5		MK_U8(85)

/** NV12 format (YUV420 semi-planar: Y10,V10U10) */
#define VI_PIXFMT_FORMAT_T_Y10__V10U10_N420	MK_U8(98)

/** NV21 format (YUV420 semi-planar: Y10,U10V10) */
#define VI_PIXFMT_FORMAT_T_Y10__U10V10_N420	MK_U8(99)

/** YV12 format (YUV420 full planar: Y10,U10,V10) */
#define VI_PIXFMT_FORMAT_T_Y10__U10__V10_N420	MK_U8(100)

/** NV16 format (YUV422 semi-planar: Y10,V10U10) */
#define VI_PIXFMT_FORMAT_T_Y10__V10U10_N422	MK_U8(101)

/** NV61 format (YUV422 semi-planar: Y10,U10V10) */
#define VI_PIXFMT_FORMAT_T_Y10__U10V10_N422	MK_U8(102)

/** YV16 format (YUV422 full planar: Y10,U10,V10) */
#define VI_PIXFMT_FORMAT_T_Y10__U10__V10_N422	MK_U8(103)

/** RLE compressed format for RAW12 data */
#define VI_PIXFMT_FORMAT_T_DPCM_RAW12		K_U32(128)

/** RAW ISP encoded float, 20-bit fixed */
#define VI_PIXFMT_FORMAT_T_R16_X_ISP20		MK_U8(194)

/** Alias for @ref VI_PIXFMT_FORMAT_T_R16_X_ISP20 */
#define VI_PIXFMT_FORMAT_T_R16_ISP		VI_PIXFMT_FORMAT_T_R16_X_ISP20

/** NvColorFormat_Float_R16 */
#define VI_PIXFMT_FORMAT_T_R16_F		MK_U8(195)

/** NvColorFormat_R16 */
#define VI_PIXFMT_FORMAT_T_R16			MK_U8(196)

/** NvColorFormat_Int_R16 */
#define VI_PIXFMT_FORMAT_T_R16_I		MK_U8(197)

/** NvColorFormat_R16_X_ISP24 (RAW ISP encoded float, 24-bit fixed) */
#define VI_PIXFMT_FORMAT_T_R16_X_ISP24		MK_U8(198)

/** NvColorFormat_R24 */
#define VI_PIXFMT_FORMAT_T_R24			MK_U8(210)

/** NvColorFormat_R32 */
#define VI_PIXFMT_FORMAT_T_R32			MK_U8(230)

/** NvColorFormat_Float_R32 */
#define VI_PIXFMT_FORMAT_T_R32_F		MK_U8(232)

/** RLE compressed format for RAW16 data */
#define VI_PIXFMT_FORMAT_T_DPCM_RAW16		MK_U8(254)

/** RLE compressed format for RAW20 data */
#define VI_PIXFMT_FORMAT_T_DPCM_RAW20		MK_U8(255)
/**@}*/


/**
 * @brief VI Channel configuration
 *
 * Parameters for VI unit register programming to capture a frame.
 */
struct vi_channel_config {
	/** Enable CSI datatype override (1=enable, 0=disable). */
	unsigned dt_enable:1;

	/** Enable reception of sensor embedded data (1=enable, 0=disable). */
	unsigned embdata_enable:1;

	/** Enable memory flush function (1=enable, 0=disable). */
	unsigned flush_enable:1;

	/** Enable periodic (subframe) memory flushes (1=enable, 0=disable). */
	unsigned flush_periodic:1;

	/** Enable line timer function (1=enable, 0=disable). */
	unsigned line_timer_enable:1;

	/** Periodic line timer notice enabled flag (1=enable, 0=disable). */
	unsigned line_timer_periodic:1;

	/** Enable PIXFMT writing pixels flag (1=enable, 0=disable). */
	unsigned pixfmt_enable:1;

	/**
	 * Enable merging of adjacent RAW8, RAW10, or RAW12 pixel values
	 * into a single wide pixel (1=enable, 0=disable).
	 */
	unsigned pixfmt_wide_enable:1;

	/** Set wide pixel endianness (0 = big endian, 1 = little endian). */
	unsigned pixfmt_wide_endian:1;

	/**
	 * Enable Phase Detection Auto Focus (PDAF) pixel replacement
	 * (1=enable, 0=disable). (non-safety)
	 */
	unsigned pixfmt_pdaf_replace_enable:1;

	/** ISPA buffer enabled  (1=enable, 0=disable). (deprecated, non-safety) */
	unsigned ispbufa_enable:1;

	/** ISPB buffer enabled  (1=enable, 0=disable). (deprecated, non-safety) */
	unsigned ispbufb_enable:1;

	/** Enable pixel companding (1=enable, 0=disable). (non-safety) */
	unsigned compand_enable:1;

	/** Reserved bits */
	unsigned pad_flags__:19;

	struct match_rec {
		/**
		 * @ref NvCsiDataType "CSI datatype" value to match.
		 */
		uint8_t datatype;

		/**
		 * Bitmask for bits of @ref datatype to compare [0, UINT8_MAX].
		 * Normally set to UINT8_MAX to not restrict the comparison.
		 */
		uint8_t datatype_mask;

		/**
		 * 1-hot encoded @ref NvCsiStream "NVCSI stream" to match
		 * (1 << @ref NvCsiStream "stream").
		 */
		uint8_t stream;

		/**
		 * Bitmask for bits of @ref stream to compare [0, 0x3F].
		 * Normally set to 0x3F to not restrict the comparison.
		 */
		uint8_t stream_mask;

		/**
		 * 1-hot encoded @ref NvCsiVirtualChannel "CSI Virtual Channel"
		 * to match (1 << @ref NvCsiVirtualChannel "VC").
		 */
		uint16_t vc;

		/**
		 * Bitmask for bits of @ref vc to compare [0, UINT16_MAX].
		 * Normally set to UINT16_MAX to not restrict the comparison.
		 */
		uint16_t vc_mask;

		/** CSI frame number to match. */
		uint16_t frameid;

		/**
		 * Bitmask for bits of @ref frameid to compare [0, UINT16_MAX].
		 * Normally set to 0 to disable frame matching.
		 */
		uint16_t frameid_mask;

		/**
		 * For Sony sensors with Digital Overlap mode:
		 * data in the DOL header to match [0, UINT16_MAX].
		 */
		uint16_t dol;

		/**
		 * Bitmask for bits of @ref dol to compare [0, UINT16_MAX].
		 * Normally set to 0 to disable DOL matching.
		 */
		uint16_t dol_mask;
	} match;
	/**<
	 * VI channel match parameters. Used to select incoming frames
	 * for this VI channel.
	 */

	/**
	 * For Sony sensors with Digital Overlap mode: DOL header select [0, 3].
	 * Configures which pixel in the first pixel packet will carry the
	 * DOL header used for channel matching (see @ref match::dol).
	 */
	uint8_t dol_header_sel;

	/**
	 * @ref NvCsiDataType "Datatype" override value. If @ref dt_enable is
	 * set this value overrides the incoming pixel datatype from the sensor.
	 * This setting can be used to capture user defined datatypes, for
	 * example.
	 */
	uint8_t dt_override;

	/** @ref ViDpcmModes "VI DPCM Mode". (non-safety) */
	uint8_t dpcm_mode;

	/** Reserved */
	uint8_t pad_dol_dt_dpcm__;

	struct vi_frame_config {
		/**
		 * Frame width (pixels) before cropping [0, UINT16_MAX].
		 * 0 is interpreted as 65536.
		 */
		uint16_t frame_x;

		/**
		 * Frame height (lines) before cropping [0, UINT16_MAX].
		 * 0 is interpreted as 65536.
		 */
		uint16_t frame_y;

		/** Number of embedded data bytes on a line [0, 131071]. */
		uint32_t embed_x;

		/** Number of embedded data lines in a frame [0, 65535]. */
		uint32_t embed_y;

		struct skip_rec {
			/**
			 * Left edge of the crop region, counted in groups
			 * of 8 pixels [0, MIN(@ref frame_x - 1,
			 * @ref crop_rec::x - 1) / 8]. Left crop is disabled
			 * if the value is 0.
			 */
			uint16_t x;
			/**
			 * Top edge of crop region counted in lines
			 * [0, MIN(@ref frame_y - 1, @ref crop_rec::y - 1)].
			 * Top crop is disabled if the value is 0.
			 */
			uint16_t y;
		} skip; /**< Top left corner of crop region */

		struct crop_rec {
			/**
			 * Right edge of crop region counted in pixels
			 * [0, UINT16_MAX]. 0 is interpreted as 65536. Right
			 * crop is disabled if the crop region reaches beyond
			 * the right edge of the frame (>= @ref frame_x).
			 */
			uint16_t x;

			/**
			 * Bottom edge of crop region counted in lines
			 * [0, UINT16_MAX]. Bottom crop is disabled if the crop
			 * region reaches beyond the bottom edge of the frame
			 * (>= @ref frame_y).
			 */
			uint16_t y;
		} crop; /**< Bottom right corner of crop region */
	} frame;
	/**<
	 * Frame configuration (frame resolution, crop region,
	 * number of embedded data lines)
	 */

	/**
	 * Flush to memory after @em flush pixel lines [0, UINT16_MAX].
	 * 0 is interpreted as 65536. @ref flush_enable must be set for this
	 * setting to take effect. @ref flush_periodic must be set in order to
	 * repeat the memory flush every @em flush lines.
	 */
	uint16_t flush;

	/**
	 * Number of pixel lines before first memory flush [0, UINT16_MAX].
	 * Can be used to trigger the first memory flush on a line different
	 * from @ref flush. 0 disables this setting.
	 */
	uint16_t flush_first;

	/**
	 * Pixel line count between memory line timer events [0, UINT16_MAX].
	 * 0 is interpreted as 65536. @ref flush_enable must be set
	 * for this setting to take effect.
	 */
	uint16_t line_timer;

	/** Line count at which to trip the first line timer event */
	uint16_t line_timer_first;

	struct pixfmt_rec {
		/** @ref ViPixFmt "VI Output Pixel Format". */
		uint16_t format;

		/**
		 * Zero padding control for RAW8/10/12/14->T_R16 and
		 * RAW20/24->T_R32. 0 = disable, 1 = right alined MSB padded,
		 * 2 = left aligned LSB padded.
		 */
		uint8_t pad0_en;

		/** Reserved */
		uint8_t pad__;

		struct pdaf_rec {
			/**
			 * Within a line, X pixel position at which PDAF
			 * separation begins [0, UINT16_MAX].
			 */
			uint16_t crop_left;

			/**
			 * Within a line, X pixel position at which PDAF
			 * separation ends [0, UINT16_MAX].
			 */
			uint16_t crop_right;

			/**
			 * Line at which PDAF separation begins
			 * [0, UINT16_MAX].
			 */
			uint16_t crop_top;

			/**
			 * line at which PDAF separation ends
			 * [0, UINT16_MAX].
			 */
			uint16_t crop_bottom;

			/**
			 * Within a line, X pixel position at which PDAF
			 * replacement begins [0, UINT16_MAX].
			 */
			uint16_t replace_crop_left;

			/**
			 * Within a line, X pixel position at which PDAF
			 * replacement ends [0, UINT16_MAX].
			 */
			uint16_t replace_crop_right;

			/**
			 * Line at which PDAF replacement begins
			 * [0, UINT16_MAX].
			 */
			uint16_t replace_crop_top;

			/**
			 * Line at which PDAF repalcement ends
			 * [0, UINT16_MAX].
			 */
			uint16_t replace_crop_bottom;

			/**
			 * X coordinate of last PDAF pixel within the PDAF
			 * crop window [0, UINT16_MAX]. */
			uint16_t last_pixel_x;

			/**
			 * Y coordinate of last PDAF pixel within the PDAF
			 * crop window [0, UINT16_MAX].
			 */
			uint16_t last_pixel_y;

			/** Value to replace PDAF pixel with */
			uint16_t replace_value;

			/**
			 * Memory format for writing PDAF pixels. Only
			 * T_R16_X_ISP20, T_R16_F, T_R16, T_R32, and T_R32_F
			 * formats are supported (see @ref ViPixFmt
			 * "VI Output Pixel Formats" for values).
			 */
			uint8_t format;

			/** Reserved */
			uint8_t pad_pdaf__;
		} pdaf;
		/**<
		 * Configuration for Phase Detection Auto-Focus pixels.
		 * (non-safety)
		 */

	} pixfmt; /**< Pixel format configuration */

	struct dpcm_rec {
		/**
		 * Number of pixels in the strip [0, UINT16_MAX] (@deprecated).
		 * */
		uint16_t strip_width;

		/**
		 * Number of pixel packets in overfetch region [0, UINT16_MAX].
		 * 0 disables overfetch.
		 */
		uint16_t strip_overfetch;

		/**
		 * Number of pixel packets in first generated chunk
		 * (no OVERFETCH region) [0, UINT16_MAX].
		 * */
		uint16_t chunk_first;

		/**
		 * Number of pixel packets in “body” chunks (including
		 * possible overfetch) [0, UINT16_MAX].
		 */
		uint16_t chunk_body;

		/** Number of “body” chunks to emit [0, UINT16_MAX]. */
		uint16_t chunk_body_count;

		/**
		 * Number of pixel packets in chunk immediately after “body”
		 * chunks (including possible overfecth) [0, UINT16_MAX].,
		 */
		uint16_t chunk_penultimate;

		/**
		 * Number of pixel packets in the final generated chunk
		 * (including possible overfetch) [0, UINT16_MAX].
		 */
		uint16_t chunk_last;

		/** Reserved */
		uint16_t pad__;

		/** Maximum value to truncate input data to [0, 0xFFFFF]. */
		uint32_t clamp_high;

		/** Minimum value to truncate input data to [0, 0xFFFFF]. */
		uint32_t clamp_low;

	} dpcm; /**< Configuration for DPCM compression. (non-safety) */

	struct atomp_rec {
		struct surface_rec {
			/**
			 * Undefined in RCE-FW interface.
			 *
			 * In the UMD-KMD shared memory interface this carries
			 * an offset into the memory buffer identified by
			 * a memory handle stored in @ref offset_hi.
			 */
			uint32_t offset;

			/**
			 * Undefined in RCE-FW interface.
			 *
			 * In the UMD-KMD interface this carries a memory
			 * handle for the output buffer. See KMD documentation
			 * for the constraints.
			 */
			uint32_t offset_hi;

		} surface[VI_NUM_ATOMP_SURFACES];
		/**<
		 * Memory buffers for output surfaces. Currently only defined
		 * for the shared memory interface between UMD and KMD. The
		 * content is undefined for the RCE-FW interface.
		 */

		/** Line stride of the surface in bytes [0, 0xFFFFF]. */
		uint32_t surface_stride[VI_NUM_ATOMP_SURFACES];

		/**
		 * DPCM chunk stride (distance from start of chunk to end of
		 * chunk) [0, 0xFFFFF].
		 * */
		uint32_t dpcm_chunk_stride;
	} atomp;
	/**< Memory output configuration. */

	/** Reserved */
	uint16_t pad__[2];

} CAPTURE_IVC_ALIGN;

/**
 * @brief Memory buffer for engine status.
 *
 * This structure is used to between UMD and KMD to share a buffer
 * for writing VI or ISP engine status.
 */
struct engine_status_surface {
	/**
	 * Undefined in RCE-FW interface.
	 *
	 * In the UMD-KMD interface this carries an Offset into a memory
	 * buffer identified by the buffer handle stored in @ref offset_hi.
	 *
	 * Valid only in UMD-KMD interface.
	 */
	uint32_t offset;

	/**
	 * Undefined in RCE-FW interface.
	 *
	 * In the UMD-KMD inteface this carries a memory handle for the
	 * engine stauts buffer. Must be a valid buffer handle or 0.
	 */
	uint32_t offset_hi;
} CAPTURE_IVC_ALIGN;

/**
 * @brief Watermark offset for specifying address within watermark ring buffer.
 */
struct watermark_mem_offset {
	/** Index within watermark buffer */
	uint32_t buff_idx;
	/** Size of watermark */
	uint32_t size;
} CAPTURE_IVC_ALIGN;

/**
 * @brief NVCSI error status
 *
 * Detailed error information reported by the CSI interface.
 *
 * This information is provided for logging and triage of any capture
 * issues. The information is queried from the CSI interface whenever
 * a capture request completes. However, it should be noted that the
 * information is not frame accurate or even channel accurate in some
 * cases.
 *
 * Any error reported here must not be construed to mean
 * that the captured frame is good or bad. Please refer to
 * @ref capture_status::status for making that judgement.
 *
 */
struct nvcsi_error_status {
	/**
	 * NVCSI @ref NvCsiStreamErrors "stream errors" map to a CSI port
	 * and affect all capture channels sharing that port via the use
	 * of CSI virtual channels.
	 *
	 * The errors will only be reported only once, on the capture channel
	 * which happened to retrieve the error report first, not on all
	 * capture channels sharing the CSI port.
	 *
	 * Note that these errors will cause VI to independently detect
	 * errors on all affected channels and the error reporting done
	 * by VI is frame accurate, unlike CSI, so the client need not
	 * consider these errors for normal operation.
	 */
	uint32_t nvcsi_stream_bits;

	/**
	 * @defgroup NvCsiStreamErrors NVCSI Stream error bits
	 */
	/** @{ */

	/**
	 * Both CRC values in a CPHY CSI packet header have a mismatch.
	 * The packet header is corrupted.
	 */
#define NVCSI_STREAM_ERR_STAT_PH_BOTH_CRC_ERR			MK_BIT32(1)

	/**
	 * Multiple bit errors detected by in DPHY CSI packet header by ECC.
	 * The packet header is corrupted.
	 */
#define NVCSI_STREAM_ERR_STAT_PH_ECC_MULTI_BIT_ERR		MK_BIT32(0)
	/** @} */

	/**
	 * NVCSI @ref NvcsiVirtualChannelErrors "virtual channel errors" map
	 * to a single CSI virtual channel carried over a CSI port. These
	 * errors are also forwarded to VI with each frame-end packet and
	 * reported to the client with each affected capture request as
	 * part of @ref notify_bits.
	 */
	uint32_t nvcsi_virtual_channel_bits;

	/**
	 * @defgroup NvCsiVirtualChannelErrors NVCSI Virtual Channel error bits
	 */
	/** @{ */

	/**
	 * CSI embedded data line CRC error. The sensor embedded data
	 * is corrupted.
	 */
#define NVCSI_VC_ERR_INTR_STAT_EMBEDDED_LINE_CRC_ERR	MK_BIT32(5)

	/**
	 * One out of two CRCs for the CPHY CSI packet header mismatches.
	 * The CRC value itself is corrupted.  This error does not
	 * cause frame corruption.
	 */
#define NVCSI_VC_ERR_INTR_STAT_PH_SINGLE_CRC_ERR	MK_BIT32(4)

	/**
	 * CSI payload data word count short error. The received data line
	 * was incomplete.
	 */
#define NVCSI_VC_ERR_INTR_STAT_PD_WC_SHORT_ERR		MK_BIT32(3)

	/**
	 * A CRC mismatch was detected in CSI packet payload data.
	 * The payload is corrupted.
	 */
#define NVCSI_VC_ERR_INTR_STAT_PD_CRC_ERR		MK_BIT32(2)

	/**
	 * A single bit error was corrected by ECC in the DPHY CSI
	 * packet header.
	 */
#define NVCSI_VC_ERR_INTR_STAT_PH_ECC_SINGLE_BIT_ERR	MK_BIT32(1)

	/**
	 * CSI pixel parser finite state machine timeout.
	 * A line of data was not received in time
	 * (see @ref CAPTURE_CSI_STREAM_SET_PARAM_REQ_MSG::watchdog_config).
	 */
#define NVCSI_VC_ERR_INTR_STAT_PPFSM_TIMEOUT		MK_BIT32(0)
	/** @} */

	/**
	 * NVCSI @ref NvCsiCilErrors "common interface logic errors"
	 * reported by partition A of the CSI brick (CIL A).
	 */
	uint32_t cil_a_error_bits;

	/**
	 * NVCSI @ref NvCsiCilErrors "common interface logic errors"
	 * reported by partition B of the CSI brick (CIL B).
	 */
	uint32_t cil_b_error_bits;

	/**
	 * @defgroup NvCsiCilErrors NVCSI CIL error bits
	 */
	/** @{ */

	/** Escape mode sync error on lane 1. */
#define NVCSI_ERR_CIL_DATA_LANE_ESC_MODE_SYNC_ERR1	MK_BIT32(14)

	/** Escape mode sync error on lane 0. */
#define NVCSI_ERR_CIL_DATA_LANE_ESC_MODE_SYNC_ERR0	MK_BIT32(13)

	/**
	 * Lane alignment error. Some lanes detected a sync word while
	 * other lanes did not.
	 */
#define NVCSI_ERR_DPHY_CIL_LANE_ALIGN_ERR		MK_BIT32(12)

	/**
	 * DPHY skew calibration did not complete while sweeping the
	 * clock lane trimmer. This will happen if the calibration
	 * sequence is not long enough.
	 */
#define NVCSI_ERR_DPHY_CIL_DESKEW_CALIB_ERR_CTRL	MK_BIT32(11)

	/**
	 * DPHY skew calibration did not complete while sweeping the
	 * data lane 1 trimmer. This will happen if the calibration
	 * sequence is not long enough.
	 */
#define NVCSI_ERR_DPHY_CIL_DESKEW_CALIB_ERR_LANE1	MK_BIT32(10)

	/**
	 * DPHY skew calibration did not complete while sweeping the
	 * data lane 0 trimmer. This will happen if the calibration
	 * sequence is not long enough.
	 */
#define NVCSI_ERR_DPHY_CIL_DESKEW_CALIB_ERR_LANE0	MK_BIT32(9)

	/** Data lane 1 receive FIFO overflow. */
#define NVCSI_ERR_CIL_DATA_LANE_RXFIFO_FULL_ERR1	MK_BIT32(8)

	/** LP sequence error detected on DATA lane 1. */
#define NVCSI_ERR_CIL_DATA_LANE_CTRL_ERR1		MK_BIT32(7)

	/** Multiple bit errors detected in the data lane 1 sync word. */
#define NVCSI_ERR_CIL_DATA_LANE_SOT_MB_ERR1		MK_BIT32(6)

	/** Single bit error detected in the lane 1 sync word. */
#define NVCSI_ERR_CIL_DATA_LANE_SOT_SB_ERR1		MK_BIT32(5)

	/** Data lane 0 receive FIFO overflow. */
#define NVCSI_ERR_CIL_DATA_LANE_RXFIFO_FULL_ERR0	MK_BIT32(4)

	/** LP sequence error detected on DATA lane 0. */
#define NVCSI_ERR_CIL_DATA_LANE_CTRL_ERR0		MK_BIT32(3)

	/** Multiple bit errors detected in the data lane 0 sync word */
#define NVCSI_ERR_CIL_DATA_LANE_SOT_MB_ERR0             MK_BIT32(2)

	/** Single bit error detected in the lane 0 sync word. */
#define NVCSI_ERR_CIL_DATA_LANE_SOT_SB_ERR0		MK_BIT32(1)

	/** DPHY clock lane control error */
#define NVCSI_ERR_DPHY_CIL_CLK_LANE_CTRL_ERR		MK_BIT32(0)
	/** @} */
};

/**
 * @brief Frame capture status record
 *
 * The capture status is part of @ref capture_descriptor and is populated
 * by RCE-FW when the capture request is completed. The status record
 * is guaranteed to be up-to-date before RCE-FW sends the @ref
 * CAPTURE_STATUS_IND message to the client in order to signal request
 * completion.
 */
struct capture_status {
	/** @ref NvCsiStream "CSI Stream Number". */
	uint8_t src_stream;

	/** @ref NvCsiVirtualChannel "CSI virtual channel number". */
	uint8_t virtual_channel;

	/** Frame number from sensor. Value range depends on the sensor. */
	uint16_t frame_id;

	/** @ref CaptureStatusCodes "Capture status code". */
	uint32_t status;

/**
 * @defgroup CaptureStatusCodes Capture status codes
 */

/** @{ */
/**
 * Capture status unknown. This usually means that the capture request
 * is not yet executed or is still in progress. The values of other
 * fields in the capture status record are undefined.
 */
#define CAPTURE_STATUS_UNKNOWN			MK_U32(0)

/**
 * Capture status success.
 *
 * Value of @ref notify_bits may be checked for any
 * non frame-corrupting errors detected during capture.
 */
#define CAPTURE_STATUS_SUCCESS			MK_U32(1)

/**
 * CSIMUX frame error. Maps to VI CSIMUX_FRAME notify event.
 *
 * This signals an error detected by VI at frame boundary or
 * a CSI error received with FE packet from NVCSI.
 *
 * Refer to @ref notify_bits for error details.
 */
#define CAPTURE_STATUS_CSIMUX_FRAME		MK_U32(2)

/**
 * CSIMUX stream error. Maps to VI CSIMUX_STREAM notify event.
 *
 * An error was detected in the VI CSIMUX stream interface.
 *
 *
 * Refer to @ref notify_bits for error details.
 */
#define CAPTURE_STATUS_CSIMUX_STREAM		MK_U32(3)

/**
 * Unused. @deprecated
 *
 * CHANSEL_FAULT errors are reported with @ref
 * CAPTURE_STATUS_FALCON_ERROR.
 */
#define CAPTURE_STATUS_CHANSEL_FAULT		MK_U32(4)

/**
 * Unused. @deprecated
 *
 * CHANSEL_FAULT_FE errors are reported with @ref
 * CAPTURE_STATUS_FALCON_ERROR.
 */
#define CAPTURE_STATUS_CHANSEL_FAULT_FE		MK_U32(5)

/**
 * Channel collision error. Maps to VI CHANSEL_COLLISION
 * notify event.
 *
 * A received start-of-frame packet matches a VI channel that
 * is already receiving a frame. This is typically caused by
 * a mistake in programming the
 * (@ref capture_descriptor::match "channel match rules")
 * so that they match more than one sensor stream.
 *
 * This error sets @ref CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_COLLISION
 * in @ref notify_bits.
 */
#define CAPTURE_STATUS_CHANSEL_COLLISION	MK_U32(6)

/**
 * Unused. @deprecated.
 *
 * CHANSEL_SHORT_FRAME errors are reported with @ref
 * CAPTURE_STATUS_FALCON_ERROR.
 */
#define CAPTURE_STATUS_CHANSEL_SHORT_FRAME	MK_U32(7)

/**
 * Single ATOMP surface packer has overflowed. Maps to VI
 * ATOMP_PACKER_OVERFLOW notify event. This is an internal
 * error that should not happen in normal use.
 *
 * This error sets @ref CAPTURE_STATUS_NOTIFY_BIT_ATOMP_PACKER_OVERFLOW
 * in @ref notify_bits.
 */
#define CAPTURE_STATUS_ATOMP_PACKER_OVERFLOW	MK_U32(8)

/**
 * Frame truncated writing to system memory. Maps to VI
 * ATOMP_FRAME_TRUNCATED event. This error typically indicates
 * memory back-pressure due to insufficient memory bandwidth
 * or an excessive memory access latency.
 *
 * This error sets @ref CAPTURE_STATUS_NOTIFY_BIT_ATOMP_FRAME_TRUNCATED
 * in @ref notify_bits.
 */
#define CAPTURE_STATUS_ATOMP_FRAME_TRUNCATED	MK_U32(9)

/**
 * Frame discarded writing to system memory. Maps to VI
 * ATOMP_FRAME_TOSSED event. This error typically indicates
 * memory back-pressure due to insufficient memory bandwidth
 * or an excessive memory access latency.
 *
 * This error sets @ref CAPTURE_STATUS_NOTIFY_BIT_ATOMP_FRAME_TOSSED
 * in @ref notify_bits.
 */
#define CAPTURE_STATUS_ATOMP_FRAME_TOSSED	MK_U32(10)

/**
 * Unused. @deprecated
 *
 * ISPBUF FIFO interfaces are not used.
 */
#define CAPTURE_STATUS_ISPBUF_FIFO_OVERFLOW	MK_U32(11)

/**
 * Capture status out of sync. A stale error from a past frame was
 * detected and logged. An unknown number of frames may have been lost
 * due to, for example, a longer break in the sensor connection.
 */
#define CAPTURE_STATUS_SYNC_FAILURE		MK_U32(12)

/**
 * Unused. @deprecated
 */
#define CAPTURE_STATUS_NOTIFIER_BACKEND_DOWN	MK_U32(13)

/**
 * Capture error reported by VI falcon. See @ref notify_bits
 * for detailed @ref ViFalconErrorBits "errors".
 */
#define CAPTURE_STATUS_FALCON_ERROR		MK_U32(14)

/**
 * Incoming frame does not match any active channel.
 * Maps to VI CHANSEL_NOMATCH event.
 *
 * This typically means that the client has either not configured
 * a capture channel for the sensor or has been unable to supply
 * capture requests quickly enough, causing a request queue
 * underflow. Each CHANSEL_NOMATCH error implies that a frame
 * has been dropped.
 */
#define CAPTURE_STATUS_CHANSEL_NOMATCH		MK_U32(15)

/**
 * Invalid VI capture settings. Parameter validation error from
 * VI driver.
 */
#define CAPTURE_STATUS_INVALID_CAP_SETTINGS	MK_U32(16)

/** @} */

	/** TSC based start-of-frame (SOF) timestamp (ns) */
	uint64_t sof_timestamp;

	/** TSC based end-of-frame (EOF) timestamp (ns) */
	uint64_t eof_timestamp;

	/**
	 * @deprecated
	 *
	 * See @ref notify_bits for error details.
	 */
	uint32_t err_data;

/**
 * @defgroup CaptureStatusFlags Capture status flags
 */
/** @{ */
	/** Channel encountered unrecoverable error and must be reset */
#define CAPTURE_STATUS_FLAG_CHANNEL_IN_ERROR			MK_BIT32(1)
/** @} */

	/** See @ref CaptureStatusFlags "Capture status flags" */
	uint32_t flags;

	/**
	 * @ref ViNotifyErrorTag "VI errors" logged in capture channel since
	 * previous capture.
	 */
	uint64_t notify_bits;

	/**
	 * @defgroup ViNotifyErrorTag VI errors
	 *
	 * VI error bits for the @ref notify_bits field.
	 */
	/** @{ */

	/**
	 * Frame start fault. A FS packet was received while already
	 * in frame indicating that a FE packet was lost.
	 *
	 * This is not a frame corrupting error, unless pixel data was
	 * lost as well, and may also be present in the capture status
	 * of a successful request.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_FS_FAULT				MK_BIT64(2)

	/** FE forced by CSIMUX stream reset or stream timeout. */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_FORCE_FE_FAULT			MK_BIT64(3)

	/** Frame ID of the FE packet does not match the Frame ID of the FS packet.  */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_FE_FRAME_ID_FAULT		MK_BIT64(4)

	/** An illegal pixel enable encoding was detected in a long packet. */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_PXL_ENABLE_FAULT			MK_BIT64(5)

	/**
	 * CSI pixel parser finite state machine timeout.
	 * A line of data was not received in time
	 * (see @ref CAPTURE_CSI_STREAM_SET_PARAM_REQ_MSG::watchdog_config).
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_CSI_FAULT_PPFSM_TIMEOUT		MK_BIT64(15)

	/**
	 * A single bit error was corrected by ECC in the DPHY CSI
	 * packet header.
	 *
	 * This is not a frame corrupting error and may also be present
	 * in the capture status of a successful request.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_CSI_FAULT_PH_ECC_SINGLE_BIT_ERR	MK_BIT64(16)

	/**
	 * A CRC mismatch was detected in CSI packet payload data.
	 * The payload is corrupted.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_CSI_FAULT_PD_CRC_ERR		MK_BIT64(17)

	/**
	 * CSI payload data word count short error. The received data line
	 * was incomplete.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_CSI_FAULT_PD_WC_SHORT_ERR	MK_BIT64(18)

	/**
	 * One out of two CRCs for the CPHY CSI packet header mismatches.
	 * The CRC value itself is corrupted.  This error does not cause
	 * frame corruption.
	 *
	 * This is not a frame corrupting error and may also be present
	 * in the capture status of a successful request.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_CSI_FAULT_PH_SINGLE_CRC_ERR	MK_BIT64(19)

	/**
	 * CSI embedded data line CRC error. The sensor embedded data
	 * is corrupted.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_FRAME_CSI_FAULT_EMBEDDED_LINE_CRC_ERR	MK_BIT64(20)

	/**
	 * Spurious data detected between valid frames or before first frame.
	 * This can be a badly corrupted frame or some random bits. This error
	 * doesn't have an effect on the next captured frame. Spurious data
	 * before the first captured frame is often caused by receiving the
	 * tail part of a partial frame in the beginning. Spurious errors
	 * detected before first full frame will be suppressed.
	 *
	 * This is not a frame corrupting error and may also be present
	 * in the capture status of a subsequent successful request.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_STREAM_SPURIOUS_DATA			MK_BIT64(21)

	/**
	 * Stream FIFO overflow. This error is unrecoverable. The FIFO
	 * overflow error usually implies incorrect configuration of
	 * VI and ISP core clock rates. This error should not happen
	 * during normal use.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_STREAM_FIFO_OVERFLOW			MK_BIT64(22)

	/**
	 * Stream loss of frame error. A FS packet was lost due a stream
	 * FIFO overflow. This error is unrecoverable. The FIFO
	 * overflow error usually implies incorrect configuration of
	 * VI and ISP core clock rates. This error should not happen
	 * during normal use.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_STREAM_FIFO_LOF			MK_BIT64(23)

	/**
	 * An illegal data packet was encountered and dropped by CSIMUX.
	 * This error may have no effect on capture result or may trigger
	 * other errors if the frame got corrupted.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CSIMUX_STREAM_FIFO_BADPKT			MK_BIT64(24)

	/**
	 * Start of frame capture timed out. The timeout is measured
	 * from VI falcon task activation to frame start (see
	 * @ref capture_descriptor::frame_start_timeout).
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_FRAME_START_TIMEOUT			MK_BIT64(25)

	/**
	 * Frame capture timed out. The timeout is measured from frame start
	 * to frame completion (see @ref capture_descriptor::frame_timeout).
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_FRAME_COMPLETION_TIMEOUT		MK_BIT64(26)

	/** Missing line-end packet in pixel data line */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_PIXEL_MISSING_LE		MK_BIT64(30)

	/** Frame has too many lines */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_PIXEL_RUNAWAY			MK_BIT64(31)

	/** Pixel data received without line-start packet */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_PIXEL_SPURIOUS		MK_BIT64(32)

	/** Pixel line is too long */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_PIXEL_LONG_LINE		MK_BIT64(33)

	/** Pixel line is too short */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_PIXEL_SHORT_LINE		MK_BIT64(34)

	/** Missing line-end packet in embedded data line */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_EMBED_MISSING_LE		MK_BIT64(35)

	/** Frame has too many lines of embedded data */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_EMBED_RUNAWAY			MK_BIT64(36)

	/** Embedded data received without line-start packet */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_EMBED_SPURIOUS		MK_BIT64(37)

	/** Embedded data line is too long */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_EMBED_LONG_LINE		MK_BIT64(38)

	/**
	 * Embedded data received when not expected. Sensor has been
	 * programmed to send embedded data but the capture has
	 * not been programmed to expect it.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_EMBED_INFRINGE		MK_BIT64(39)

	/** Invalid pixel datatype in pixel packet or line-start packet */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_DTYPE_MISMATCH		MK_BIT64(40)

	/** Short frame: frame has too few pixel lines */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_PIX_SHORT			MK_BIT64(42)

	/** Short frame: frame has too few embedded data lines */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_EMB_SHORT			MK_BIT64(43)

	/**
	 * VI hardware failure detected by
	 * (@ref CAPTURE_CHANNEL_FLAG_ENABLE_VI_PFSD).
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_PFSD_FAULT				MK_BIT64(44)

	/**
	 * Frame was truncated due to a frame timeout, or
	 * a channel reset or channel release operation.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_FAULT_FE			MK_BIT64(45)

	/**
	 * Incoming frame was not matched by any VI channel.This error is
	 * usually caused by not having a pending capture request ready to
	 * catch the incoming frame. The frame will be dropped. This often
	 * indicates a request starvation condition that may be caused by
	 * performance issues on the client side.
	 *
	 * This is not a frame corrupting error and may also be present
	 * in the capture status of a subsequent successful request.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_NO_MATCH			MK_BIT64(46)

	/**
	 * More than one VI channel match the same incoming frame.
	 * Two or more channels have been configured to catch frames
	 * from the same source. Only one of the channels will capture
	 * the frame and the other one will report this error for the
	 * conflicting capture request.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_COLLISION			MK_BIT64(47)

	/**
	 * Channel reconfigured while in frame. This is not an error and
	 * this status should not been seen in practice.  @deprecated
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_CHANSEL_LOAD_FRAMED			MK_BIT64(48)

	/**
	 * Internal overflow in ATOMP packer. This is Internal
	 * hardware error that should not occur in normal use.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_ATOMP_PACKER_OVERFLOW			MK_BIT64(49)

	/**
	 * Frame truncated while writing to system memory.
	 * Indicates memory back-pressure due to insufficient memory
	 * bandwidth or an excessive memory access latency.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_ATOMP_FRAME_TRUNCATED			MK_BIT64(50)

	/**
	 * Frame dropped while writing to system memory.
	 * Indicates memory back-pressure due to insufficient memory
	 * bandwidth or an excessive memory access latency.
	 */
	#define CAPTURE_STATUS_NOTIFY_BIT_ATOMP_FRAME_TOSSED			MK_BIT64(51)

	/** Unclassified error. Should not happen. */
	#define CAPTURE_STATUS_NOTIFY_BIT_UNCLASSIFIED_ERROR			MK_BIT64(63)

	/** Unclassified error. Should not happen. @deprecated */
	#define CAPTURE_STATUS_NOTIFY_BIT_NON_CLASSIFIED_0			MK_BIT64(63)
	/** @} */

	/**
	 * NVCSI error status.
	 *
	 * Error bits representing errors which were reported by NVCSI since
	 * previous capture.
	 *
	 * Multiple errors of same kind are collated into single bit.
	 *
	 * NVCSI error status is likely, but not guaranteed to affect current frame:
	 *
	 * 1. NVCSI error status is retrieved at end-of-frame VI event. NVCSI may already
	 * retrieve next frame data at this time.
	 *
	 * 2. NVCSI Error may also indicate error from older CSI data if there were frame
	 * skips between captures.
	 *
	 */
	struct nvcsi_error_status nvcsi_err_status;

} CAPTURE_IVC_ALIGN;

/**
 * @brief The compand configuration describes a piece-wise linear
 * tranformation function used by the VI companding module.
 */
#define VI_NUM_COMPAND_KNEEPTS MK_SIZE(10)
struct vi_compand_config {
	/** Input position for this knee point */
	uint32_t base[VI_NUM_COMPAND_KNEEPTS];
	/** Scale above this knee point */
	uint32_t scale[VI_NUM_COMPAND_KNEEPTS];
	/** Output offset for this knee point */
	uint32_t offset[VI_NUM_COMPAND_KNEEPTS];
} CAPTURE_IVC_ALIGN;

/*
 * @brief VI Phase Detection Auto Focus (PDAF) configuration
 *
 * The PDAF data consists of special pixels that will be extracted from a frame
 * and written to a separate surface. The PDAF pattern is shared by all capture channels
 * and should be configured before enabling PDAF pixel extraction for a specific capture.
 *
 * Pixel { x, y } will be ouput to the PDAF surface (surface1) if the
 * bit at position (x % 32) in pattern[y % 32] is set.
 *
 * Pixel { x, y } in the main output surface (surface0) will be
 * replaced by a default pixel value if the bit at position (x % 32)
 * in pattern_replace[y % 32] is set.
 */
#define VI_PDAF_PATTERN_SIZE 32
struct vi_pdaf_config {
	/**
	 * Pixel bitmap, by line PATTERN[y0][x0] is set if the pixel (x % 32) == x0, (y % 32) == y0
	 * should be output to the PDAF surface
	 */
	uint32_t pattern[VI_PDAF_PATTERN_SIZE];
	/**
	 * Pixel bitmap to be used for Replace the pdaf pixel, by line
	 * PATTERN_REPLACE[y0][x0] is set if the pixel (x % 32) == x0, (y % 32) == y0
	 * should be output to the PDAF surface
	 */
	uint32_t pattern_replace[VI_PDAF_PATTERN_SIZE];
} CAPTURE_IVC_ALIGN;

/*
 * @brief VI SYNCGEN unit configuration.
 */
struct vi_syncgen_config {
	/**
	 * Half cycle - Unsigned floating point.
	 * Decimal point position is given by FRAC_BITS in HCLK_DIV_FMT.
	 * Frequency of HCLK = SYNCGEN_CLK / (HALF_CYCLE * 2)
	 */
	uint32_t hclk_div;
	/** Number of fractional bits of HALF_CYCLE */
	uint8_t hclk_div_fmt;
	/** Horizontal sync signal */
	uint8_t xhs_width;
	/** Vertical sync signal */
	uint8_t xvs_width;
	/** Cycles to delay after XVS before assert XHS */
	uint8_t xvs_to_xhs_delay;
	/** Resevred - UNUSED */
	uint16_t cvs_interval;
	/** Reserved */
	uint16_t pad1__;
	/** Reserved */
	uint32_t pad2__;
} CAPTURE_IVC_ALIGN;

/**
 * @brief VI PFSD Configuration.
 *
 * The PDAF replacement function is used in PFSD mode. Pixels within a
 * designated region are replaced by a test pattern, and output pixels
 * from the region are compared against expected values after a capture.
 */
struct vi_pfsd_config {
	/**
	 * Region within which the pixels are replaced with a test pattern.
	 */
	struct replace_roi_rec {
		/**
		 * left pixel column of the replacement region [0, UINT16_MAX].
		 */
		uint16_t left;

		/**
		 * right pixel column of the replacement region (inclusive)
		 * [0, UINT16_MAX].
		 */
		uint16_t right;

		/** top pixel row of the replacement region [0, UINT16_MAX]. */
		uint16_t top;

		/**
		 * bottom pixel row of the replacement region (inclusive)
		 * [0, UINT16_MAX].
		 */
		uint16_t bottom;

	} replace_roi; /**< Pixel replacement region. */

	/**
	 * The test pattern used to replace pixels within the region
	 * [0, 0xFFFFF]. The value is defined in the nvcsi2vi pixel
	 * bus format. The actual value range depends on the
	 * @ref NvCsiDataType "CSI datatype".
	 */
	uint32_t replace_value;

	/**
	 * Count of items in the @ref expected array
	 * [0, VI_NUM_VI_PFSD_SURFACES]. If zero, PFSD
	 * will not be performed for this frame
	 */
	uint32_t expected_count;

	/**
	 * Array of area definitions for output surfaces that will be
	 * verified. For YUV422 semi-planar, [0] is the Y surface and
	 * [1] ist the UV surface.
	 */
	struct {
		/**
		 * Byte offset for the roi from beginning of the surface
		 * [0, UINT32_MAX].
		 */
		uint32_t offset;

		/**
		 * Number of bytes that need to be read from the output
		 * surface [0, 255].
		 */
		uint32_t len;

		/**
		 * Sequence of expected values [0, UINT8_MAX]. The 4 byte
		 * pattern is repeated until @ref len bytes have been
		 * compared.
		 */
		uint8_t value[4];
	} expected[VI_NUM_VI_PFSD_SURFACES]; /**< Expected PFSD values. */

} CAPTURE_IVC_ALIGN;

/**
 * @defgroup CaptureRequestFlags Capture request flags
 */
/** @{ */

/** Enable capture status and error reporting for the channel. */
#define CAPTURE_FLAG_STATUS_REPORT_ENABLE	MK_BIT32(0)

/**
 * Enable error reporting only for the channel. No effect if
 * @ref CAPTURE_FLAG_STATUS_REPORT_ENABLE is set.
 */
#define CAPTURE_FLAG_ERROR_REPORT_ENABLE	MK_BIT32(1)
/** @} */

/**
 * @brief Memory surface specs passed from KMD to RCE
 */
struct memoryinfo_surface {
	/**
	 * Surface IOVA address. Must be a multiple of 16.
	 * Must be non-zero if @ref size > 0.
	 */
	uint64_t base_address;

	/** Surface size. Must be a multiple of 16. */
	uint64_t size;
};

/**
 * @brief VI capture descriptor memory information
 *
 * VI capture descriptor memory information shared between
 * KMD and RCE only. This information cannot be part of
 * capture descriptor since descriptor is shared with usermode
 * application.
 */
struct capture_descriptor_memoryinfo {
	/** VI output surfaces */
	struct memoryinfo_surface surface[VI_NUM_ATOMP_SURFACES];

	/**
	 * Base IOVA of engine status surface. Must be a multiple of 16.
	 * Must be non-zero if @ref engine_status_surface_size > 0.
	 */
	uint64_t engine_status_surface_base_address;

	/**
	 * Size of engine status surface.
	 * Must be >= @ref NV_ENGINE_STATUS_SURFACE_SIZE.
	 */
	uint64_t engine_status_surface_size;

	/** Memory surface for watermark ring buffer written by VI FW */
	struct memoryinfo_surface watermark_surface;

	/** Reserved */
	uint32_t reserved32[8];
} CAPTURE_DESCRIPTOR_ALIGN;

/**
 * @brief VI frame capture context.
 */
struct capture_descriptor {
	/** Capture request sequence number [0, UINT32_MAX]. */
	uint32_t sequence;

	/** See @ref CaptureRequestFlags "capture request flags". */
	uint32_t capture_flags;

	/**
	 * Task descriptor frame start timeout in milliseconds
	 * [0, UINT16_MAX].
	 */
	uint16_t frame_start_timeout;

	/**
	 * Task descriptor frame completion timeout in milliseconds
	 * [0, UINT16_MAX].
	 */
	uint16_t frame_completion_timeout;

#define CAPTURE_PREFENCE_ARRAY_SIZE		2

	/** @deprecated  */
	uint32_t prefence_count CAMRTC_DEPRECATED;
	/** @deprecated */
	struct syncpoint_info prefence[CAPTURE_PREFENCE_ARRAY_SIZE] CAMRTC_DEPRECATED;

	/** VI Channel configuration. */
	struct vi_channel_config ch_cfg;

	/**
	 * VI PFSD Configuration.
	 * The @ref CAPTURE_CHANNEL_FLAG_ENABLE_VI_PFSD is set in
	 * @ref channel_flags for this setting to take effect.
	 */
	struct vi_pfsd_config pfsd_cfg;

	/**
	 * Engine status surface for downstream synchronization.
	 * Engine status is written by VI when a frame is complete.
	 *
	 * This is only valid in the UMD-KMD interface.
	 * The content is undefined in the RCE FW interface.
	 */
	struct engine_status_surface engine_status;

	/**
	 * Capture status record. Written by RCE when the capture
	 * request is completed.
	 */
	struct capture_status status;

	/** Unique ID for the output buffer used for watermarking */
	uint64_t output_buffer_id;

	/** Offset for the next watermark within the watermark surface */
	struct watermark_mem_offset watermark_offset;

	/** Reserved */
	uint32_t pad32__[10];

} CAPTURE_DESCRIPTOR_ALIGN;

/**
 * @brief - Event data used for event injection
 */
struct event_inject_msg {
	/** UMD populates with capture status events. RCE converts to reg offset */
	uint32_t tag;
	/** Timestamp of event */
	uint32_t stamp;
	/** Bits [0:31] of event data */
	uint32_t data;
	/** Bits [32:63] of event data */
	uint32_t data_ext;
};

/**
 * @defgroup ViChanselErrMask VI CHANSEL error numbers
 * @{
 */
#define VI_HSM_CHANSEL_ERROR_MASK_BIT_NOMATCH MK_U32(1)
/** @} */

/**
 * @brief VI EC/HSM global CHANSEL error masking
 */
struct vi_hsm_chansel_error_mask_config {
	/**
	 * This error mask applies only to errors that are masked in @ref
	 * chansel_uncorrectable_mask. By default, all such errors are reported
	 * to System Error Handler as corrected. If a specific error is masked
	 * in both this mask and in @ref error_mask_uncorrectable, by setting
	 * the corresponding bit (1 << <em>error number</em>) in the error mask
	 * (see @ref ViChanselErrMask "VI CHANSEL error numbers"), the error
	 * will not be reported to System Error Handler at all.
	 *
	 * A corrected safety error will be interpreted by System Error Handler
	 * as an indication that a single frame has been corrupted or dropped.
	 */
	uint32_t chansel_correctable_mask;
	/**
	 * Error mask for suppressing uncorrected safety errors (see @ref
	 * ViChanselErrMask "VI CHANSEL error numbers"). If the mask is set
	 * to zero, VI CHANSEL errors will be reported as uncorrected safety
	 * errors. If a specific error is masked by setting the corresponding
	 * bit (1 << <em>error number</em>) in the error mask, the error will
	 * not be reported as uncorrected. Note that the error may still be
	 * reported as a corrected error unless it is also masked in
	 * @ref chansel_correctable_mask.
	 *
	 * An uncorrected error will be interpreted by System Error Handler
	 * as an indication that a camera is off-line and cannot continue
	 * the capture.
	 */
	uint32_t chansel_uncorrectable_mask;
} CAPTURE_IVC_ALIGN;

/**
 * NvPhy attributes
 */
/**
 * @defgroup NvPhyType
 * NvCSI Physical stream type
 * @{
 */
#define NVPHY_TYPE_CSI		MK_U32(0)
#define NVPHY_TYPE_SLVSEC	MK_U32(1)
/**@}*/

/**
 * NVCSI attributes
 */
/**
 * @defgroup NvCsiPort NvCSI Port
 * @{
 */
/** Port A maps to @ref NVCSI_STREAM_0 */
#define NVCSI_PORT_A		MK_U32(0x0)
/** Port B maps to @ref NVCSI_STREAM_1 */
#define NVCSI_PORT_B		MK_U32(0x1)
/** Port C maps to @ref NVCSI_STREAM_2 */
#define NVCSI_PORT_C		MK_U32(0x2)
/** Port D maps to @ref NVCSI_STREAM_3 */
#define NVCSI_PORT_D		MK_U32(0x3)
/** Port E maps to @ref NVCSI_STREAM_4 */
#define NVCSI_PORT_E		MK_U32(0x4)
/** Port F maps to @ref NVCSI_STREAM_4 with a custom lane swizzle configuration */
#define NVCSI_PORT_F		MK_U32(0x5)
/** Port G maps to @ref NVCSI_STREAM_5 */
#define NVCSI_PORT_G		MK_U32(0x6)
/** Port H maps to @ref NVCSI_STREAM_5 with a custom lane swizzle configuration */
#define NVCSI_PORT_H		MK_U32(0x7)
/** Port not specified. */
#define NVCSI_PORT_UNSPECIFIED	MK_U32(0xFFFFFFFF)
/**@}*/

/**
 * @defgroup NvCsiStream NVCSI stream id
 * @{
 */
#define NVCSI_STREAM_0		MK_U32(0x0)
#define NVCSI_STREAM_1		MK_U32(0x1)
#define NVCSI_STREAM_2		MK_U32(0x2)
#define NVCSI_STREAM_3		MK_U32(0x3)
#define NVCSI_STREAM_4		MK_U32(0x4)
#define NVCSI_STREAM_5		MK_U32(0x5)
/**@}*/

/**
 * @defgroup NvCsiVirtualChannel NVCSI virtual channels
 * @{
 */
#define NVCSI_VIRTUAL_CHANNEL_0		MK_U32(0x0)
#define NVCSI_VIRTUAL_CHANNEL_1		MK_U32(0x1)
#define NVCSI_VIRTUAL_CHANNEL_2		MK_U32(0x2)
#define NVCSI_VIRTUAL_CHANNEL_3		MK_U32(0x3)
#define NVCSI_VIRTUAL_CHANNEL_4		MK_U32(0x4)
#define NVCSI_VIRTUAL_CHANNEL_5		MK_U32(0x5)
#define NVCSI_VIRTUAL_CHANNEL_6		MK_U32(0x6)
#define NVCSI_VIRTUAL_CHANNEL_7		MK_U32(0x7)
#define NVCSI_VIRTUAL_CHANNEL_8		MK_U32(0x8)
#define NVCSI_VIRTUAL_CHANNEL_9		MK_U32(0x9)
#define NVCSI_VIRTUAL_CHANNEL_10	MK_U32(0xA)
#define NVCSI_VIRTUAL_CHANNEL_11	MK_U32(0xB)
#define NVCSI_VIRTUAL_CHANNEL_12	MK_U32(0xC)
#define NVCSI_VIRTUAL_CHANNEL_13	MK_U32(0xD)
#define NVCSI_VIRTUAL_CHANNEL_14	MK_U32(0xE)
#define NVCSI_VIRTUAL_CHANNEL_15	MK_U32(0xF)
/**@}*/

/**
 * @defgroup NvCsiConfigFlags NvCSI Configuration Flags
 * @{
 */
/** NVCSI config flags */
#define NVCSI_CONFIG_FLAG_BRICK		MK_BIT32(0)
/** NVCSI config flags */
#define NVCSI_CONFIG_FLAG_CIL		MK_BIT32(1)
/** Enable user-provided error handling configuration */
#define NVCSI_CONFIG_FLAG_ERROR		MK_BIT32(2)
/**@}*/

/**
 * @brief Number of lanes/trios per brick
 */
#define NVCSI_BRICK_NUM_LANES	MK_U32(4)
/**
 * @brief Number of override exception data types
 */
#define NVCSI_NUM_NOOVERRIDE_DT	MK_U32(5)

/**
 * @defgroup NvCsiPhyType NVCSI physical types
 * @{
 */
/** NVCSI D-PHY physical layer */
#define NVCSI_PHY_TYPE_DPHY	MK_U32(0)
/** NVCSI D-PHY physical layer */
#define NVCSI_PHY_TYPE_CPHY	MK_U32(1)
/** @} */

/**
 * @defgroup NvCsiLaneSwizzle NVCSI lane swizzles
 * @{
 */
/** 00000 := A0 A1 B0 B1 -->  A0 A1 B0 B1 */
#define NVCSI_LANE_SWIZZLE_A0A1B0B1	MK_U32(0x00)
/** 00001 := A0 A1 B0 B1 -->  A0 A1 B1 B0 */
#define NVCSI_LANE_SWIZZLE_A0A1B1B0	MK_U32(0x01)
/** 00010 := A0 A1 B0 B1 -->  A0 B0 B1 A1 */
#define NVCSI_LANE_SWIZZLE_A0B0B1A1	MK_U32(0x02)
/** 00011 := A0 A1 B0 B1 -->  A0 B0 A1 B1 */
#define NVCSI_LANE_SWIZZLE_A0B0A1B1	MK_U32(0x03)
/** 00100 := A0 A1 B0 B1 -->  A0 B1 A1 B0 */
#define NVCSI_LANE_SWIZZLE_A0B1A1B0	MK_U32(0x04)
/** 00101 := A0 A1 B0 B1 -->  A0 B1 B0 A1 */
#define NVCSI_LANE_SWIZZLE_A0B1B0A1	MK_U32(0x05)
/** 00110 := A0 A1 B0 B1 -->  A1 A0 B0 B1 */
#define NVCSI_LANE_SWIZZLE_A1A0B0B1	MK_U32(0x06)
/** 00111 := A0 A1 B0 B1 -->  A1 A0 B1 B0 */
#define NVCSI_LANE_SWIZZLE_A1A0B1B0	MK_U32(0x07)
/** 01000 := A0 A1 B0 B1 -->  A1 B0 B1 A0 */
#define NVCSI_LANE_SWIZZLE_A1B0B1A0	MK_U32(0x08)
/** 01001 := A0 A1 B0 B1 -->  A1 B0 A0 B1 */
#define NVCSI_LANE_SWIZZLE_A1B0A0B1	MK_U32(0x09)
/** 01010 := A0 A1 B0 B1 -->  A1 B1 A0 B0 */
#define NVCSI_LANE_SWIZZLE_A1B1A0B0	MK_U32(0x0A)
/** 01011 := A0 A1 B0 B1 -->  A1 B1 B0 A0 */
#define NVCSI_LANE_SWIZZLE_A1B1B0A0	MK_U32(0x0B)
/** 01100 := A0 A1 B0 B1 -->  B0 A1 A0 B1 */
#define NVCSI_LANE_SWIZZLE_B0A1A0B1	MK_U32(0x0C)
/** 01101 := A0 A1 B0 B1 -->  B0 A1 B1 A0 */
#define NVCSI_LANE_SWIZZLE_B0A1B1A0	MK_U32(0x0D)
/** 01110 := A0 A1 B0 B1 -->  B0 A0 B1 A1 */
#define NVCSI_LANE_SWIZZLE_B0A0B1A1	MK_U32(0x0E)
/** 01111 := A0 A1 B0 B1 -->  B0 A0 A1 B1 */
#define NVCSI_LANE_SWIZZLE_B0A0A1B1	MK_U32(0x0F)
/** 10000 := A0 A1 B0 B1 -->  B0 B1 A1 A0 */
#define NVCSI_LANE_SWIZZLE_B0B1A1A0	MK_U32(0x10)
/** 10001 := A0 A1 B0 B1 -->  B0 B1 A0 A1 */
#define NVCSI_LANE_SWIZZLE_B0B1A0A1	MK_U32(0x11)
/** 10010 := A0 A1 B0 B1 -->  B1 A1 B0 A0 */
#define NVCSI_LANE_SWIZZLE_B1A1B0A0	MK_U32(0x12)
/** 10011 := A0 A1 B0 B1 -->  B1 A1 A0 B0 */
#define NVCSI_LANE_SWIZZLE_B1A1A0B0	MK_U32(0x13)
/** 10100 := A0 A1 B0 B1 -->  B1 B0 A0 A1 */
#define NVCSI_LANE_SWIZZLE_B1B0A0A1	MK_U32(0x14)
/** 10101 := A0 A1 B0 B1 -->  B1 B0 A1 A0 */
#define NVCSI_LANE_SWIZZLE_B1B0A1A0	MK_U32(0x15)
/** 10110 := A0 A1 B0 B1 -->  B1 A0 A1 B0 */
#define NVCSI_LANE_SWIZZLE_B1A0A1B0	MK_U32(0x16)
/** 10111 := A0 A1 B0 B1 -->  B1 A0 B0 A1 */
#define NVCSI_LANE_SWIZZLE_B1A0B0A1	MK_U32(0x17)
/** @} */

/**
 * @defgroup NvCsiDPhyPolarity NVCSI D-phy Polarity
 * @{
 */
#define NVCSI_DPHY_POLARITY_NOSWAP	MK_U32(0)
#define NVCSI_DPHY_POLARITY_SWAP	MK_U32(1)
/** @} */

/**
 * @defgroup NvCsiCPhyPolarity NVCSI C-phy Polarity
 * @{
 */
/* 000 := A B C --> A B C */
#define NVCSI_CPHY_POLARITY_ABC	MK_U32(0x00)
/* 001 := A B C --> A C B */
#define NVCSI_CPHY_POLARITY_ACB	MK_U32(0x01)
/* 010 := A B C --> B C A */
#define NVCSI_CPHY_POLARITY_BCA	MK_U32(0x02)
/* 011 := A B C --> B A C */
#define NVCSI_CPHY_POLARITY_BAC	MK_U32(0x03)
/* 100 := A B C --> C A B */
#define NVCSI_CPHY_POLARITY_CAB	MK_U32(0x04)
/* 101 := A B C --> C B A */
#define NVCSI_CPHY_POLARITY_CBA	MK_U32(0x05)
/** @} */

/**
 * @brief NvCSI Brick configuration
 */
struct nvcsi_brick_config {
	/** Select PHY @ref NvCsiPhyType "mode" for both partitions */
	uint32_t phy_mode;
	/** See @ref NvCsiLaneSwizzle "NVCSI Lane swizzles" control
	 * for bricks. Valid for C-PHY and D-PHY modes.
	 */
	uint32_t lane_swizzle;
	/**
	 * Polarity control for each lane. Value depends on @a phy_mode.
	 * See @ref NvCsiDPhyPolarity "NVCSI D-phy Polarity"
	 * or @ref NvCsiCPhyPolarity "NVCSI C-phy Polarity"
	 */
	uint8_t lane_polarity[NVCSI_BRICK_NUM_LANES];
	/** Reserved */
	uint32_t pad32__;
} CAPTURE_IVC_ALIGN;

/**
 * @brief NvCSI Control and Interface Logic Configuration
 */
struct nvcsi_cil_config {
	/** Number of data lanes used (0-4) */
	uint8_t num_lanes;
	/** LP bypass mode (boolean) */
	uint8_t lp_bypass_mode;
	/** Set MIPI THS-SETTLE timing (LP clock cycles with SoC default clock rate) */
	uint8_t t_hs_settle;
	/** Set MIPI TCLK-SETTLE timing (LP clock cycles with SoC default clock rate) */
	uint8_t t_clk_settle;
	/** @deprecated  */
	uint32_t cil_clock_rate CAMRTC_DEPRECATED;
	/** MIPI clock rate for D-Phy. Symbol rate for C-Phy [kHz] */
	uint32_t mipi_clock_rate;
	/** Reserved */
	uint32_t pad32__;
} CAPTURE_IVC_ALIGN;

/**
 * @defgroup HsmCsimuxErrors Bitmask for CSIMUX errors reported to HSM
 */
/** @{ */
/** Error bit indicating next packet after a frame end was not a frame start */
#define VI_HSM_CSIMUX_ERROR_MASK_BIT_SPURIOUS_EVENT MK_BIT32(0)
/** Error bit indicating FIFO for the stream has over flowed */
#define VI_HSM_CSIMUX_ERROR_MASK_BIT_OVERFLOW MK_BIT32(1)
/** Error bit indicating frame start packet lost due to FIFO overflow */
#define VI_HSM_CSIMUX_ERROR_MASK_BIT_LOF MK_BIT32(2)
/** Error bit indicating that an illegal packet has been sent from NVCSI */
#define VI_HSM_CSIMUX_ERROR_MASK_BIT_BADPKT MK_BIT32(3)
/** @} */

/**
 * @brief VI EC/HSM error masking configuration
 */
struct vi_hsm_csimux_error_mask_config {
	/** Mask correctable CSIMUX. See @ref HsmCsimuxErrors "CSIMUX error bitmask". */
	uint32_t error_mask_correctable;
	/** Mask uncorrectable CSIMUX. See @ref HsmCsimuxErrors "CSIMUX error bitmask". */
	uint32_t error_mask_uncorrectable;
} CAPTURE_IVC_ALIGN;

/**
 * @defgroup NVCSI_HOST1X_INTR_FLAGS NVCSI Host1x client global interrupt flags
 * @{
 */
/** Error bit indicating Host1x client timeout error */
#define NVCSI_INTR_FLAG_HOST1X_TIMEOUT_ERR			MK_BIT32(0)
/** @} */

/**
 * @defgroup NVCSI_STREAM_INTR_FLAGS NVCSI stream novc+vc interrupt flags
 * @{
 */
/** Multi bit error in the DPHY packet header */
#define NVCSI_INTR_FLAG_STREAM_NOVC_ERR_PH_ECC_MULTI_BIT	MK_BIT32(0)
/** Error bit indicating both of the CPHY packet header CRC check fail */
#define NVCSI_INTR_FLAG_STREAM_NOVC_ERR_PH_BOTH_CRC		MK_BIT32(1)
/** Error bit indicating VC Pixel Parser (PP) FSM timeout for a pixel line.*/
#define NVCSI_INTR_FLAG_STREAM_VC_ERR_PPFSM_TIMEOUT		MK_BIT32(2)
/** Error bit indicating VC has packet with single bit ECC error in the packet header*/
#define NVCSI_INTR_FLAG_STREAM_VC_ERR_PH_ECC_SINGLE_BIT		MK_BIT32(3)
/** Error bit indicating VC has packet payload crc check fail */
#define NVCSI_INTR_FLAG_STREAM_VC_ERR_PD_CRC			MK_BIT32(4)
/** Error bit indicating VC has packet terminate before getting the expect word count data. */
#define NVCSI_INTR_FLAG_STREAM_VC_ERR_PD_WC_SHORT		MK_BIT32(5)
/** Error bit indicating VC has one of the CPHY packet header CRC check fail. */
#define NVCSI_INTR_FLAG_STREAM_VC_ERR_PH_SINGLE_CRC		MK_BIT32(6)
/** @} */

/**
 * @defgroup NVCSI_CIL_INTR_FLAGS NVCSI phy/cil interrupt flags
 * @{
 */
#define NVCSI_INTR_FLAG_CIL_INTR_DPHY_ERR_CLK_LANE_CTRL		MK_BIT32(0)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR0_SOT_SB		MK_BIT32(1)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR0_SOT_MB		MK_BIT32(2)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR0_CTRL		MK_BIT32(3)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR0_RXFIFO_FULL	MK_BIT32(4)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR1_SOT_SB		MK_BIT32(5)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR1_SOT_MB		MK_BIT32(6)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR1_CTRL		MK_BIT32(7)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR1_RXFIFO_FULL	MK_BIT32(8)
#define NVCSI_INTR_FLAG_CIL_INTR_DPHY_DESKEW_CALIB_ERR_LANE0	MK_BIT32(9)
#define NVCSI_INTR_FLAG_CIL_INTR_DPHY_DESKEW_CALIB_ERR_LANE1	MK_BIT32(10)
#define NVCSI_INTR_FLAG_CIL_INTR_DPHY_DESKEW_CALIB_ERR_CTRL	MK_BIT32(11)
#define NVCSI_INTR_FLAG_CIL_INTR_DPHY_LANE_ALIGN_ERR		MK_BIT32(12)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR0_ESC_MODE_SYNC	MK_BIT32(13)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR1_ESC_MODE_SYNC	MK_BIT32(14)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR0_SOT_2LSB_FULL	MK_BIT32(15)
#define NVCSI_INTR_FLAG_CIL_INTR_DATA_LANE_ERR1_SOT_2LSB_FULL	MK_BIT32(16)
/** @} */

/**
 * @defgroup NVCSI_CIL_INTR0_FLAGS NVCSI phy/cil intr0 flags
 * @{
 */
#define NVCSI_INTR_FLAG_CIL_INTR0_DPHY_ERR_CLK_LANE_CTRL	MK_BIT32(0)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR0_SOT_SB		MK_BIT32(1)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR0_SOT_MB		MK_BIT32(2)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR0_CTRL		MK_BIT32(3)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR0_RXFIFO_FULL	MK_BIT32(4)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR1_SOT_SB		MK_BIT32(5)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR1_SOT_MB		MK_BIT32(6)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR1_CTRL		MK_BIT32(7)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR1_RXFIFO_FULL	MK_BIT32(8)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR0_SOT_2LSB_FULL	MK_BIT32(9)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR1_SOT_2LSB_FULL	MK_BIT32(10)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR0_ESC_MODE_SYNC	MK_BIT32(19)
#define NVCSI_INTR_FLAG_CIL_INTR0_DATA_LANE_ERR1_ESC_MODE_SYNC	MK_BIT32(20)
#define NVCSI_INTR_FLAG_CIL_INTR0_DPHY_DESKEW_CALIB_DONE_LANE0	MK_BIT32(22)
#define NVCSI_INTR_FLAG_CIL_INTR0_DPHY_DESKEW_CALIB_DONE_LANE1	MK_BIT32(23)
#define NVCSI_INTR_FLAG_CIL_INTR0_DPHY_DESKEW_CALIB_DONE_CTRL	MK_BIT32(24)
#define NVCSI_INTR_FLAG_CIL_INTR0_DPHY_DESKEW_CALIB_ERR_LANE0	MK_BIT32(25)
#define NVCSI_INTR_FLAG_CIL_INTR0_DPHY_DESKEW_CALIB_ERR_LANE1	MK_BIT32(26)
#define NVCSI_INTR_FLAG_CIL_INTR0_DPHY_DESKEW_CALIB_ERR_CTRL	MK_BIT32(27)
#define NVCSI_INTR_FLAG_CIL_INTR0_DPHY_LANE_ALIGN_ERR		MK_BIT32(28)
#define NVCSI_INTR_FLAG_CIL_INTR0_CPHY_CLK_CAL_DONE_TRIO0	MK_BIT32(29)
#define NVCSI_INTR_FLAG_CIL_INTR0_CPHY_CLK_CAL_DONE_TRIO1	MK_BIT32(30)
/** @} */

/**
 * @defgroup NVCSI_CIL_INTR1_FLAGS NVCSI phy/cil intr1 flags
 * @{
 */
#define NVCSI_INTR_FLAG_CIL_INTR1_DATA_LANE_ESC_CMD_REC0	MK_BIT32(0)
#define NVCSI_INTR_FLAG_CIL_INTR1_DATA_LANE_ESC_DATA_REC0	MK_BIT32(1)
#define NVCSI_INTR_FLAG_CIL_INTR1_DATA_LANE_ESC_CMD_REC1	MK_BIT32(2)
#define NVCSI_INTR_FLAG_CIL_INTR1_DATA_LANE_ESC_DATA_REC1	MK_BIT32(3)
#define NVCSI_INTR_FLAG_CIL_INTR1_REMOTERST_TRIGGER_INT0	MK_BIT32(4)
#define NVCSI_INTR_FLAG_CIL_INTR1_ULPS_TRIGGER_INT0		MK_BIT32(5)
#define NVCSI_INTR_FLAG_CIL_INTR1_LPDT_INT0			MK_BIT32(6)
#define NVCSI_INTR_FLAG_CIL_INTR1_REMOTERST_TRIGGER_INT1	MK_BIT32(7)
#define NVCSI_INTR_FLAG_CIL_INTR1_ULPS_TRIGGER_INT1		MK_BIT32(8)
#define NVCSI_INTR_FLAG_CIL_INTR1_LPDT_INT1			MK_BIT32(9)
#define NVCSI_INTR_FLAG_CIL_INTR1_DPHY_CLK_LANE_ULPM_REQ	MK_BIT32(10)
/** @} */

/**
 * @defgroup NVCSI_INTR_CONFIG_MASK NVCSI interrupt config bit masks
 * @{
 */
#define NVCSI_INTR_CONFIG_MASK_HOST1X		MK_U32(0x1)
#define NVCSI_INTR_CONFIG_MASK_STATUS2VI	MK_U32(0xffff)
#define NVCSI_INTR_CONFIG_MASK_STREAM_NOVC	MK_U32(0x3)
#define NVCSI_INTR_CONFIG_MASK_STREAM_VC	MK_U32(0x7c)
#define NVCSI_INTR_CONFIG_MASK_CIL_INTR		MK_U32(0x1ffff)
#define NVCSI_INTR_CONFIG_MASK_CIL_INTR0	MK_U32(0x7fd807ff)
#define NVCSI_INTR_CONFIG_MASK_CIL_INTR1	MK_U32(0x7ff)
/** @} */

/**
 * @defgroup NVCSI_INTR_CONFIG_MASK_SHIFTS NVCSI interrupt config bit shifts
 * @{
 */
#define NVCSI_INTR_CONFIG_SHIFT_STREAM_NOVC	MK_U32(0x0)
#define NVCSI_INTR_CONFIG_SHIFT_STREAM_VC	MK_U32(0x2)
/** @} */

/**
 * @brief User-defined error configuration.
 *
 * Flag NVCSI_CONFIG_FLAG_ERROR must be set to enable these settings,
 * otherwise default settings will be used.
 */
struct nvcsi_error_config {
	/**
	 * @brief Host1x client global interrupt mask (to LIC)
	 * Bit field mapping: @ref NVCSI_HOST1X_INTR_FLAGS
	 */
	uint32_t host1x_intr_mask_lic;
	/**
	 * @brief Host1x client global interrupt mask (to HSM)
	 * Bit field mapping: @ref NVCSI_HOST1X_INTR_FLAGS
	 */
	uint32_t host1x_intr_mask_hsm;
	/**
	 * @brief Host1x client global interrupt error type classification
	 * (to HSM)
	 * Bit field mapping: @ref NVCSI_HOST1X_INTR_FLAGS
	 * (0 - corrected, 1 - uncorrected)
	 */
	uint32_t host1x_intr_type_hsm;

	/** NVCSI status2vi forwarding mask (to VI NOTIFY) */
	uint32_t status2vi_notify_mask;

	/**
	 * @brief NVCSI stream novc+vc interrupt mask (to LIC)
	 * Bit field mapping: @ref NVCSI_STREAM_INTR_FLAGS
	 */
	uint32_t stream_intr_mask_lic;
	/**
	 * @brief NVCSI stream novc+vc interrupt mask (to HSM)
	 * Bit field mapping: @ref NVCSI_STREAM_INTR_FLAGS
	 */
	uint32_t stream_intr_mask_hsm;
	/**
	 * @brief NVCSI stream novc+vc interrupt error type classification
	 * (to HSM)
	 * Bit field mapping: @ref NVCSI_STREAM_INTR_FLAGS
	 * (0 - corrected, 1 - uncorrected)
	 */
	uint32_t stream_intr_type_hsm;

	/**
	 * @brief NVCSI phy/cil interrupt mask (to HSM)
	 * Bit field mapping: @ref NVCSI_CIL_INTR_FLAGS
	 */
	uint32_t cil_intr_mask_hsm;
	/**
	 * @brief NVCSI phy/cil interrupt error type classification
	 * (to HSM)
	 * Bit field mapping: @ref NVCSI_CIL_INTR_FLAGS
	 * (0 - corrected, 1 - uncorrected)
	 */
	uint32_t cil_intr_type_hsm;
	/**
	 * @brief NVCSI phy/cil intr0 interrupt mask (to LIC)
	 * Bit field mapping: @ref NVCSI_CIL_INTR0_FLAGS
	 */
	uint32_t cil_intr0_mask_lic;
	/**
	 * @brief NVCSI phy/cil intr1 interrupt mask (to LIC)
	 * Bit field mapping: @ref NVCSI_CIL_INTR1_FLAGS
	 */
	uint32_t cil_intr1_mask_lic;

	/** Reserved */
	uint32_t pad32__;

	/** VI EC/HSM error masking configuration */
	struct vi_hsm_csimux_error_mask_config csimux_config;
} CAPTURE_IVC_ALIGN;

/**
 * @defgroup NvCsiDataType NVCSI datatypes
 * @{
 */
#define NVCSI_DATATYPE_UNSPECIFIED	MK_U32(0)
#define NVCSI_DATATYPE_YUV420_8		MK_U32(24)
#define NVCSI_DATATYPE_YUV420_10	MK_U32(25)
#define NVCSI_DATATYPE_LEG_YUV420_8	MK_U32(26)
#define NVCSI_DATATYPE_YUV420CSPS_8	MK_U32(28)
#define NVCSI_DATATYPE_YUV420CSPS_10	MK_U32(29)
#define NVCSI_DATATYPE_YUV422_8		MK_U32(30)
#define NVCSI_DATATYPE_YUV422_10	MK_U32(31)
#define NVCSI_DATATYPE_RGB444		MK_U32(32)
#define NVCSI_DATATYPE_RGB555		MK_U32(33)
#define NVCSI_DATATYPE_RGB565		MK_U32(34)
#define NVCSI_DATATYPE_RGB666		MK_U32(35)
#define NVCSI_DATATYPE_RGB888		MK_U32(36)
#define NVCSI_DATATYPE_RAW6		MK_U32(40)
#define NVCSI_DATATYPE_RAW7		MK_U32(41)
#define NVCSI_DATATYPE_RAW8		MK_U32(42)
#define NVCSI_DATATYPE_RAW10		MK_U32(43)
#define NVCSI_DATATYPE_RAW12		MK_U32(44)
#define NVCSI_DATATYPE_RAW14		MK_U32(45)
#define NVCSI_DATATYPE_RAW16		MK_U32(46)
#define NVCSI_DATATYPE_RAW20		MK_U32(47)
#define NVCSI_DATATYPE_USER_1		MK_U32(48)
#define NVCSI_DATATYPE_USER_2		MK_U32(49)
#define NVCSI_DATATYPE_USER_3		MK_U32(50)
#define NVCSI_DATATYPE_USER_4		MK_U32(51)
#define NVCSI_DATATYPE_USER_5		MK_U32(52)
#define NVCSI_DATATYPE_USER_6		MK_U32(53)
#define NVCSI_DATATYPE_USER_7		MK_U32(54)
#define NVCSI_DATATYPE_USER_8		MK_U32(55)
#define NVCSI_DATATYPE_UNKNOWN		MK_U32(64)
/** @} */

/* DEPRECATED - to be removed */
/** T210 (also exists in T186) */
#define NVCSI_PATTERN_GENERATOR_T210	MK_U32(1)
/** T186 only */
#define NVCSI_PATTERN_GENERATOR_T186	MK_U32(2)
/** T194 only */
#define NVCSI_PATTERN_GENERATOR_T194	MK_U32(3)

/* DEPRECATED - to be removed */
#define NVCSI_DATA_TYPE_Unspecified		MK_U32(0)
#define NVCSI_DATA_TYPE_YUV420_8		MK_U32(24)
#define NVCSI_DATA_TYPE_YUV420_10		MK_U32(25)
#define NVCSI_DATA_TYPE_LEG_YUV420_8		MK_U32(26)
#define NVCSI_DATA_TYPE_YUV420CSPS_8		MK_U32(28)
#define NVCSI_DATA_TYPE_YUV420CSPS_10		MK_U32(29)
#define NVCSI_DATA_TYPE_YUV422_8		MK_U32(30)
#define NVCSI_DATA_TYPE_YUV422_10		MK_U32(31)
#define NVCSI_DATA_TYPE_RGB444			MK_U32(32)
#define NVCSI_DATA_TYPE_RGB555			MK_U32(33)
#define NVCSI_DATA_TYPE_RGB565			MK_U32(34)
#define NVCSI_DATA_TYPE_RGB666			MK_U32(35)
#define NVCSI_DATA_TYPE_RGB888			MK_U32(36)
#define NVCSI_DATA_TYPE_RAW6			MK_U32(40)
#define NVCSI_DATA_TYPE_RAW7			MK_U32(41)
#define NVCSI_DATA_TYPE_RAW8			MK_U32(42)
#define NVCSI_DATA_TYPE_RAW10			MK_U32(43)
#define NVCSI_DATA_TYPE_RAW12			MK_U32(44)
#define NVCSI_DATA_TYPE_RAW14			MK_U32(45)
#define NVCSI_DATA_TYPE_RAW16			MK_U32(46)
#define NVCSI_DATA_TYPE_RAW20			MK_U32(47)
#define NVCSI_DATA_TYPE_Unknown			MK_U32(64)

/* NVCSI DPCM ratio */
#define NVCSI_DPCM_RATIO_BYPASS		MK_U32(0)
#define NVCSI_DPCM_RATIO_10_8_10	MK_U32(1)
#define NVCSI_DPCM_RATIO_10_7_10	MK_U32(2)
#define NVCSI_DPCM_RATIO_10_6_10	MK_U32(3)
#define NVCSI_DPCM_RATIO_12_8_12	MK_U32(4)
#define NVCSI_DPCM_RATIO_12_7_12	MK_U32(5)
#define NVCSI_DPCM_RATIO_12_6_12	MK_U32(6)
#define NVCSI_DPCM_RATIO_14_10_14	MK_U32(7)
#define NVCSI_DPCM_RATIO_14_8_14	MK_U32(8)
#define NVCSI_DPCM_RATIO_12_10_12	MK_U32(9)

/**
 * @defgroup NvCsiParamType NvCSI Parameter Type
 * @{
 */
#define NVCSI_PARAM_TYPE_UNSPECIFIED	MK_U32(0)
#define NVCSI_PARAM_TYPE_DPCM		MK_U32(1)
#define NVCSI_PARAM_TYPE_WATCHDOG	MK_U32(2)
/**@}*/

struct nvcsi_dpcm_config {
	uint32_t dpcm_ratio;
	uint32_t pad32__;
} CAPTURE_IVC_ALIGN;

/**
 * @brief NvCSI watchdog configuration
 */
struct nvcsi_watchdog_config {
	/** Enable/disable the pixel parser watchdog */
	uint8_t enable;
	/** Reserved */
	uint8_t pad8__[3];
	/** The watchdog timer timeout period */
	uint32_t period;
} CAPTURE_IVC_ALIGN;

/**
 * NVCSI - TPG attributes
 */
/**
 * @brief Number of vertical color bars in TPG (t186)
 */
#define NVCSI_TPG_NUM_COLOR_BARS MK_U32(8)

/**
 * @brief NvCSI test pattern generator (TPG) configuration for T186
 */
struct nvcsi_tpg_config_t186 {
	/** NvCSI stream number */
	uint8_t stream_id;
	/** DEPRECATED - to be removed */
	uint8_t stream;
	/** NvCSI virtual channel ID */
	uint8_t virtual_channel_id;
	/** DEPRECATED - to be removed */
	uint8_t virtual_channel;
	/** Initial frame number */
	uint16_t initial_frame_number;
	/** Reserved */
	uint16_t pad16__;
	/** Enable frame number generation */
	uint32_t enable_frame_counter;
	/** NvCSI datatype */
	uint32_t datatype;
	/** DEPRECATED - to be removed */
	uint32_t data_type;
	/** Width of the generated test image */
	uint16_t image_width;
	/** Height of the generated test image */
	uint16_t image_height;
	/** Pixel value for each horizontal color bar (format according to DT) */
	uint32_t pixel_values[NVCSI_TPG_NUM_COLOR_BARS];
} CAPTURE_IVC_ALIGN;

/**
 * @brief NvCsiTpgFlag Test pattern generator (TPG) flags for t194, tpg-ng
 * (Non-Safety)
 * @{
 */
#define NVCSI_TPG_FLAG_PATCH_MODE			MK_U16(1)
/** Next gen TPG sine LUT mode */
#define NVCSI_TPG_FLAG_SINE_MODE			MK_U16(2)
#define NVCSI_TPG_FLAG_PHASE_INCREMENT			MK_U16(4)
#define NVCSI_TPG_FLAG_AUTO_STOP			MK_U16(8)
/** TPG next gen feature to generate embedded data with config settings */
#define NVCSI_TPG_FLAG_EMBEDDED_PATTERN_CONFIG_INFO	MK_U16(16)
/** Next gen TPG LS/LE packet generation enable flag */
#define NVCSI_TPG_FLAG_ENABLE_LS_LE			MK_U16(32)
/** TPG next gen feature to transmit CPHY packets. DPHY is default option */
#define NVCSI_TPG_FLAG_PHY_MODE_CPHY			MK_U16(64)
/* Enable CRC check.
 * If this flag is set, NVCSI will do CRC check for CPHY packet
 * headers and ECC check for DPHY packet headers.
 * TPG doesn't support ECC generation for DPHY, so enabling this
 * flag together with DPHY can be used to trigger NVCSI errors.
 */
#define NVCSI_TPG_FLAG_ENABLE_HEADER_CRC_ECC_CHECK	MK_U16(128)
/* Enable CRC/ECC override.
 * If this flag is set, NVCSI will use override registers instead of
 * of packet headers/payload CRC fields.
 */
#define NVCSI_TPG_FLAG_ENABLE_CRC_ECC_OVERRIDE		MK_U16(256)
/* Force VI error forwarding.
 * VI error forwarding is disabled by default for DPHY mode,
 * because CRC are not generated in this mode.
 * Forcing VI error forwarding allow to trigger PD CRC error for
 * tests.
 */
#define NVCSI_TPG_FLAG_FORCE_NVCSI2VI_ERROR_FORWARDING	MK_U16(512)

/** @} */

/**
 * @brief NvCSI test pattern generator (TPG) configuration for T194
 */
struct nvcsi_tpg_config_t194 {
	/** NvCSI Virtual channel ID */
	uint8_t virtual_channel_id;
	/** NvCSI datatype */
	uint8_t datatype;
	/** @ref NvCsiTpgFlag "NvCSI TPG flag" */
	uint16_t flags;
	/** Starting framen number for TPG */
	uint16_t initial_frame_number;
	/** Maximum number for frames to be generated by TPG */
	uint16_t maximum_frame_number;
	/** Width of the generated frame in pixels */
	uint16_t image_width;
	/** Height of the generated frame in pixels */
	uint16_t image_height;
	/** Embedded data line width in bytes */
	uint32_t embedded_line_width;
	/** Line count of the embedded data before the pixel frame. */
	uint32_t embedded_lines_top;
	/** Line count of the embedded data after the pixel frame. */
	uint32_t embedded_lines_bottom;
	/* The lane count for the VC. */
	uint32_t lane_count;
	/** Initial phase */
	uint32_t initial_phase;
	/** Initial horizontal frequency for red channel */
	uint32_t red_horizontal_init_freq;
	/** Initial vertical frequency for red channel */
	uint32_t red_vertical_init_freq;
	/** Rate of change of the horizontal frequency for red channel */
	uint32_t red_horizontal_freq_rate;
	/** Rate of change of the vertical frequency for red channel */
	uint32_t red_vertical_freq_rate;
	/** Initial horizontal frequency for green channel */
	uint32_t green_horizontal_init_freq;
	/** Initial vertical frequency for green channel */
	uint32_t green_vertical_init_freq;
	/** Rate of change of the horizontal frequency for green channel */
	uint32_t green_horizontal_freq_rate;
	/** Rate of change of the vertical frequency for green channel */
	uint32_t green_vertical_freq_rate;
	/** Initial horizontal frequency for blue channel */
	uint32_t blue_horizontal_init_freq;
	/** Initial vertical frequency for blue channel */
	uint32_t blue_vertical_init_freq;
	/** Rate of change of the horizontal frequency for blue channel */
	uint32_t blue_horizontal_freq_rate;
	/** Rate of change of the vertical frequency for blue channel */
	uint32_t blue_vertical_freq_rate;
} CAPTURE_IVC_ALIGN;

/**
 * @brief next gen NvCSI test pattern generator (TPG) configuration.
 */
struct nvcsi_tpg_config_tpg_ng {
	/** NvCSI Virtual channel ID */
	uint8_t virtual_channel_id;
	/** NvCSI datatype */
	uint8_t datatype;
	/** @ref NvCsiTpgFlag "NvCSI TPG flag" */
	uint16_t flags;
	/** Starting framen number for TPG */
	uint16_t initial_frame_number;
	/** Maximum number for frames to be generated by TPG */
	uint16_t maximum_frame_number;
	/** Width of the generated frame in pixels */
	uint16_t image_width;
	/** Height of the generated frame in pixels */
	uint16_t image_height;
	/** Embedded data line width in bytes */
	uint32_t embedded_line_width;
	/** Line count of the embedded data before the pixel frame. */
	uint32_t embedded_lines_top;
	/** Line count of the embedded data after the pixel frame. */
	uint32_t embedded_lines_bottom;
	/** Initial phase */
	uint32_t initial_phase_red;
	/** Initial phase */
	uint32_t initial_phase_green;
	/** Initial phase */
	uint32_t initial_phase_blue;
	/** Initial horizontal frequency for red channel */
	uint32_t red_horizontal_init_freq;
	/** Initial vertical frequency for red channel */
	uint32_t red_vertical_init_freq;
	/** Rate of change of the horizontal frequency for red channel */
	uint32_t red_horizontal_freq_rate;
	/** Rate of change of the vertical frequency for red channel */
	uint32_t red_vertical_freq_rate;
	/** Initial horizontal frequency for green channel */
	uint32_t green_horizontal_init_freq;
	/** Initial vertical frequency for green channel */
	uint32_t green_vertical_init_freq;
	/** Rate of change of the horizontal frequency for green channel */
	uint32_t green_horizontal_freq_rate;
	/** Rate of change of the vertical frequency for green channel */
	uint32_t green_vertical_freq_rate;
	/** Initial horizontal frequency for blue channel */
	uint32_t blue_horizontal_init_freq;
	/** Initial vertical frequency for blue channel */
	uint32_t blue_vertical_init_freq;
	/** Rate of change of the horizontal frequency for blue channel */
	uint32_t blue_horizontal_freq_rate;
	/** Rate of change of the vertical frequency for blue channel */
	uint32_t blue_vertical_freq_rate;
	/** NvCSI stream number */
	uint8_t stream_id;
	/** NvCSI tpg embedded data spare0 reg settings */
	uint8_t emb_data_spare_0;
	/** NvCSI tpg embedded data spare1 reg settings */
	uint8_t emb_data_spare_1;
	/** NvCSI TPG output brightness gain */
	uint8_t brightness_gain_ratio;

	/** Fields below are used if NVCSI_TPG_FLAG_ENABLE_CRC_ECC_OVERRIDE flag is set.
	 * Format of packet header fields override_crc_ph_*:
	 * bits 0..15  - first header
	 * bits 31..16 - second header */
	/** This field is for the CPHY SOF packet first and second packet header CRC override */
	uint32_t override_crc_ph_sof;
	/** This field is for the CPHY EOF packet first and second packet header CRC override */
	uint32_t override_crc_ph_eof;
	/** This field is for the CPHY SOL packet first and second packet header CRC override */
	uint32_t override_crc_ph_sol;
	/** This field is for the CPHY EOL packet first and second packet header CRC override */
	uint32_t override_crc_ph_eol;
	/** This field is for the CPHY long packet packet first and second packet header CRC override */
	uint32_t override_crc_ph_long_packet;
	/** This field is for the long packet payload CRC override (both CPHY and DPHY) */
	uint32_t override_crc_payload;
	/** The TPG will not generate ECC for a packet. When using the TPG,
	 * SW should set the PP to skip the ecc check. To verify the ecc logic for safety BIST,
	 * SW can write a pre-calculated ECC for the TPG, when use with this mode,
	 * the TPG should generate a grescale pattern.
	 * 5:0		SOF_ECC		This field is for the SOF short packet ECC.
	 * 11:6		EOF_ECC		This field is for the EOF short packet ECC.
	 * 17:12	SOL_ECC		This field is for the SOL short packet ECC.
	 * 23:18	EOL_ECC		This field is for the EOL short packet ECC.
	 * 29:24	LINE_ECC	This field is for the long packet header ECC.
	 * */
	uint32_t override_ecc_ph;
	/** Reserved size */
	uint32_t reserved;
} CAPTURE_IVC_ALIGN;

/**
 * @brief Commong NvCSI test pattern generator (TPG) configuration
 */
union nvcsi_tpg_config {
	/** TPG configuration for T186 */
	struct nvcsi_tpg_config_t186 t186;
	/** TPG configuration for T194 */
	struct nvcsi_tpg_config_t194 t194;
	/** Next gen TPG configuration*/
	struct nvcsi_tpg_config_tpg_ng tpg_ng;
	/** Reserved size */
	uint32_t reserved[32];
};

/**
 * @brief TPG rate configuration, low level parameters
 */
struct nvcsi_tpg_rate_config {
	/** Horizontal blanking (clocks) */
	uint32_t hblank;
	/** Vertical blanking (clocks) */
	uint32_t vblank;
	/** T194 only: Interval between pixels (clocks) */
	uint32_t pixel_interval;
	/** next gen TPG only: data speed */
	uint32_t lane_speed;
} CAPTURE_IVC_ALIGN;

/**
 * ISP capture settings
 */

/**
 * @defgroup IspErrorMask ISP Channel error mask
 */
/** @{ */
#define CAPTURE_ISP_CHANNEL_ERROR_DMA_PBUF_ERR		MK_BIT32(0)
#define CAPTURE_ISP_CHANNEL_ERROR_DMA_SBUF_ERR		MK_BIT32(1)
#define CAPTURE_ISP_CHANNEL_ERROR_DMA_SEQ_ERR		MK_BIT32(2)
#define CAPTURE_ISP_CHANNEL_ERROR_FRAMEID_ERR		MK_BIT32(3)
#define CAPTURE_ISP_CHANNEL_ERROR_TIMEOUT		MK_BIT32(4)
#define CAPTURE_ISP_CHANNEL_ERROR_TASK_TIMEOUT		MK_BIT32(5)
#define CAPTURE_ISP_CHANNEL_ERROR_ALL			MK_U32(0x003F)
/** @} */

/**
 * @defgroup ISPProcessChannelFlags ISP process channel specific flags
 */
/**@{*/
/** Channel reset on error. Not implemented. */
#define CAPTURE_ISP_CHANNEL_FLAG_RESET_ON_ERROR		MK_U32(0x0001)
/**@}*/

/**
 * @brief Describes RTCPU side resources for a ISP capture pipe-line.
 */
struct capture_channel_isp_config {
	/** Unused. @deprecated */
	uint8_t channel_id;

	/** Reserved */
	uint8_t pad_chan__[3];

	/** See ISP process channel specific @ref ISPProcessChannelFlags "flags". */
	uint32_t channel_flags;

	/**
	 * Base address of the @ref isp_capture_descriptor ring buffer.
	 * The size of the buffer is @ref request_queue_depth *
	 * @ref request_size. The value must be non-zero and a multiple
	 * of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	iova_t requests;

	/**
	 * Maximum number of ISP requests in the queue [1, 240]. Determines the
	 * size of the @ref isp_capture_descriptor ring buffer (@ref requests).
	 */
	uint32_t request_queue_depth;

	/**
	 * Size of the buffer reserved for each ISP capture descriptor.
	 * Must be >= sizeof(@ref isp_capture_descriptor) and a multiple
	 * of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	uint32_t request_size;

	/**
	 * Base address of ISP program descriptor ring buffer.
	 * The size of the buffer is @ref program_queue_depth *
	 * @ref program_size. The value must be non-zero and a multiple
	 * of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	iova_t programs;

	/**
	 * Maximum number of ISP program requests in the program queue [1, 32].
	 * Determines the size of the ISP program ring buffer (@ref programs).
	 */
	uint32_t program_queue_depth;

	/** Size of each ISP process request (@ref isp_program_descriptor). */
	uint32_t program_size;

	/**
	 * ISP progress syncpoint information. The progress syncpoint
	 * is incremented whenever a slice of pixel data has been written
	 * to memory and once more when the status of the ISP task has
	 * been written to memory. The same progress syncpoint will keep
	 * incrementing for every consecutive capture request.
	 *
	 * @ref syncpoint_info::threshold must be set to the initial
	 * value of the hardware syncpoint on channel setup.
	 **/
	struct syncpoint_info progress_sp;

	/**
	 * ISP statistics syncpoint information. The statistics syncpoint
	 * is incremented once for every set of statistics specifed in
	 * @ref isp_progam::stats_aidx_flag. The same stats syncpoint will
	 * keep incrementing for every consecutive ISP request.
	 *
	 * @ref syncpoint_info::threshold must be set to the initial
	 * value of the hardware syncpoint on channel setup.
	 **/
	struct syncpoint_info stats_progress_sp;

	/**
	 * Base address of a memory mapped ring buffer containing ISP request
	 * buffer memory information. The size of the buffer is @ref
	 * request_queue_depth * @ref request_memoryinfo_size. The value must
	 * be non-zero and a multiple of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	iova_t requests_memoryinfo;

	/**
	 * Base address of a memory mapped ring buffer containing ISP program
	 * push buffer memory surfaces. The size of the ring buffer is @ref
	 * program_queue_depth * @ref program_memoryinfo_size. The value must
	 * be non-zero and a multiple of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	iova_t programs_memoryinfo;

	/**
	 * Size of the memoryinfo buffer reserved for each ISP request.
	 * Must be >= sizeof(@ref isp_capture_descriptor_memoryinfo) and
	 * a multiple of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	uint32_t request_memoryinfo_size;

	/**
	 * Size of the memoryinfo buffer reserved for each ISP program push
	 * buffer surface. Must be >= sizeof(@ref memoryinfo_surface) and
	 * a multiple of @ref CAPTURE_DESCRIPTOR_ALIGN_BYTES.
	 */
	uint32_t program_memoryinfo_size;

	/** ISP unit ID. See @ref ISPUnitIds "ISP Unit Identifiers". */
	uint32_t isp_unit_id;

#define HAVE_ISP_GOS_TABLES
	/**
	 * Number of elements in @ref isp_gos_tables array
	 * [0, @ref ISP_NUM_GOS_TABLES].
	 */
	uint32_t num_isp_gos_tables;

	/**
	 * Array of IOVA pointers to ISP Grid-of-Semaphores (GoS) tables
	 * (non-safety).
	 *
	 * GoS table configuration, if present, must be the same on all
	 * active channels. The IOVA addresses must be a multiple of 256.
	 */
	iova_t isp_gos_tables[ISP_NUM_GOS_TABLES];
} CAPTURE_IVC_ALIGN;

/**
 * @defgroup IspProcesStatus ISP process status codes
 */
/** @{ */
/** ISP frame processing status unknown */
#define CAPTURE_ISP_STATUS_UNKNOWN		MK_U32(0)
/** ISP frame processing succeeded */
#define CAPTURE_ISP_STATUS_SUCCESS		MK_U32(1)
/** ISP frame processing encountered an error */
#define CAPTURE_ISP_STATUS_ERROR		MK_U32(2)
/** @} */

/**
 * @brief ISP process request status
 */
struct capture_isp_status {
	/** ISP channel id */
	uint8_t chan_id;
	/** Reserved */
	uint8_t pad__;
	/** Frame sequence number */
	uint16_t frame_id;
	/** See @ref IspProcesStatus "ISP process status codes" */
	uint32_t status;
	/**
	 * Error status of ISP process request.
	 * Zero in case of SUCCESS, non-zero value case of ERROR.
	 * See @ref IspErrorMask "ISP Channel error mask".
	 */
	uint32_t error_mask;
	/** Reserved */
	uint32_t pad2__;
} CAPTURE_IVC_ALIGN;

/**
 * @defgroup IspProgramStatus ISP program status codes
 */
/** @{ */
/** ISP program status unknown */
#define CAPTURE_ISP_PROGRAM_STATUS_UNKNOWN	MK_U32(0)
/** ISP program was used successfully for frame processing */
#define CAPTURE_ISP_PROGRAM_STATUS_SUCCESS	MK_U32(1)
/** ISP program encountered an error */
#define CAPTURE_ISP_PROGRAM_STATUS_ERROR	MK_U32(2)
/** ISP program has expired and is not being used by any active process requests */
#define CAPTURE_ISP_PROGRAM_STATUS_STALE	MK_U32(3)
/** @} */

/**
 * @brief ISP program request status
 */
struct capture_isp_program_status {
	/** ISP channel id */
	uint8_t chan_id;
	/**
	 * The settings_id uniquely identifies the ISP program.
	 * The ID is assigned by application and copied here from
	 * the @ref isp_program_descriptor structure.
	 */
	uint8_t settings_id;
	/** Reserved */
	uint16_t pad_id__;
	/** @ref IspProgramStatus "Program status" */
	uint32_t status;
	/**
	 * Error status from last ISP process request using this ISP program.
	 * Zero in case of SUCCESS, non-zero value case of ERROR.
	 * See @ref IspErrorMask "ISP Channel error mask".
	 */
	uint32_t error_mask;
	/** Reserved */
	uint32_t pad2__;
} CAPTURE_IVC_ALIGN;

/**
 * @defgroup IspActivateFlag ISP program activation flag
 */
/** @{ */
/** Program request will when the frame sequence id reaches a certain threshold */
#define CAPTURE_ACTIVATE_FLAG_ON_SEQUENCE_ID	MK_U32(0x1)
/** Program request will be activate when the frame settings id reaches a certain threshold */
#define CAPTURE_ACTIVATE_FLAG_ON_SETTINGS_ID	MK_U32(0x2)
/** Each Process request is coupled with a Program request */
#define CAPTURE_ACTIVATE_FLAG_COUPLED		MK_U32(0x4)
/** @} */

/**
 * @brief Describes ISP program structure;
 */
struct isp_program_descriptor {
	/** ISP settings_id which uniquely identifies isp_program. */
	uint8_t settings_id;
	/**
	 * VI channel bound to the isp channel.
	 * In case of mem_isp_mem set this to CAPTURE_NO_VI_ISP_BINDING
	 */
	uint8_t vi_channel_id;
#define CAPTURE_NO_VI_ISP_BINDING MK_U8(0xFF)
	/** Reserved */
	uint8_t pad_sid__[2];
	/**
	 * Capture sequence id, frame id; Given ISP program will be used from this frame ID onwards
	 * until new ISP program does replace it.
	 */
	uint32_t sequence;

	/**
	 * Offset to memory mapped ISP program buffer from ISP program descriptor base address,
	 * which contains the ISP configs and PB1 containing HW settings. Ideally the offset is
	 * the size(ATOM aligned) of ISP program descriptor only, as each isp_program would be placed
	 * just after it's corresponding ISP program descriptor in memory.
	 */
	uint32_t isp_program_offset;
	/** Size of isp program structure */
	uint32_t isp_program_size;

	/**
	 * Base address of memory mapped ISP PB1 containing isp HW settings.
	 * This has to be 64 bytes aligned
	 */
	iova_t isp_pb1_mem;

	/** ISP program request status written by RCE */
	struct capture_isp_program_status isp_program_status;

	/** Unique ID for ISP stats buffer */
	uint64_t isp_stats_buffer_id;

	/** Unique ID for ISP program buffer */
	uint64_t isp_program_buffer_id;

	/** Activation condition for given ISP program. See @ref IspActivateFlag "Activation flags" */
	uint32_t activate_flags;

	/** Pad to aligned size */
	uint8_t pad__[4];
} CAPTURE_DESCRIPTOR_ALIGN;

/**
 * @brief ISP program size (ATOM aligned).
 *
 * NvCapture UMD makes sure to place isp_program just after above program
 * descriptor buffer for each request, so that KMD and RCE can co-locate
 * isp_program and it's corresponding program descriptor in memory.
 */
#define ISP_PROGRAM_MAX_SIZE 16512

/**
 * @brief ISP image surface info
 */
struct image_surface {
	/** Lower 32-bit of the buffer's base address */
	uint32_t offset;
	/** Upper 8-bit of the buffer's base address */
	uint32_t offset_hi;
	/** The surface stride in bytes */
	uint32_t surface_stride;
	/** Reserved */
	uint32_t pad_surf__;
} CAPTURE_IVC_ALIGN;

/**
 * @brief Output image surface info
 */
struct stats_surface {
	/** Lower 32-bit of the statistics buffer base address */
	uint32_t offset;
	/** Upper 8-bit of the statistics buffer base address */
	uint32_t offset_hi;
} CAPTURE_IVC_ALIGN;

/**
 * @brief Memory write crop region info
 */
struct isp_crop_rect {
	/** Topmost line stored in memory (inclusive) relative to the MW input image */
	uint16_t top;
	/** Bottommost line stored in memory (inclusive) relative to the MW input image */
	uint16_t bottom;
	/** Leftmost pixel stored in memory (inclusive) relative to the MW input image */
	uint16_t left;
	/** Rightmost pixel stored in memory (inclusive) relative to the MW input image */
	uint16_t right;
};

/**
 * @defgroup IspProcessFlag ISP process frame specific flags.
 */
/** @{ */
/** Enables process status reporting for the channel */
#define CAPTURE_ISP_FLAG_STATUS_REPORT_ENABLE	MK_BIT32(0)
/** Enables error reporting for the channel */
#define CAPTURE_ISP_FLAG_ERROR_REPORT_ENABLE	MK_BIT32(1)
/** Enables process and program request binding for the channel */
#define CAPTURE_ISP_FLAG_ISP_PROGRAM_BINDING	MK_BIT32(2)
/** @} */

/**
 * @brief ISP capture descriptor
 */
struct isp_capture_descriptor {
	/** Process request sequence number, frame id */
	uint32_t sequence;
	/** See @ref IspProcessFlag "ISP process frame specific flags." */
	uint32_t capture_flags;

	/** 1 MR port, max 3 input surfaces */
#define ISP_MAX_INPUT_SURFACES		MK_U32(3)

	/** Input images surfaces */
	struct image_surface input_mr_surfaces[ISP_MAX_INPUT_SURFACES];

	/**
	 * 3 MW ports, max 2 surfaces (multiplanar) per port.
	 */
#define ISP_MAX_OUTPUTS			MK_U32(3)
#define ISP_MAX_OUTPUT_SURFACES		MK_U32(2)

	struct {
		/** Memory write port output surfaces */
		struct image_surface surfaces[ISP_MAX_OUTPUT_SURFACES];
		/* TODO: Should we have here just image format enum value + block height instead?
		Dither settings would logically be part of ISP program */
		/** Image format definition for output surface */
		uint32_t image_def;
		/** Width of the output surface in pixels */
		uint16_t width;
		/** Height of the output surface in pixels */
		uint16_t height;
		/** Unique ID for the output buffer used for watermarking */
		uint64_t output_buffer_id;
	} outputs_mw[ISP_MAX_OUTPUTS];

	/** Flicker band (FB) statistics buffer */
	struct stats_surface fb_surface;
	/** Focus metrics (FM) statistics buffer */
	struct stats_surface fm_surface;
	/** Auto Focus Metrics (AFM) statistics buffer */
	struct stats_surface afm_surface;
	/** Local Average Clipping (LAC0) unit 0 statistics buffer */
	struct stats_surface lac0_surface;
	/** Local Average Clipping (LAC1) unit 1 statistics buffer */
	struct stats_surface lac1_surface;
	/** Histogram (H0) unit 0 statistics buffer */
	struct stats_surface h0_surface;
	/** Histogram (H1) unit 1 statistics buffer */
	struct stats_surface h1_surface;
	/** Histogram (H2) unit 2 statistics buffer, for ISP7 only */
	struct stats_surface h2_surface;
	/** Pixel Replacement Unit (PRU) statistics buffer */
	struct stats_surface pru_bad_surface;
	/** RAW24 Histogram Unit statistics buffer */
	struct stats_surface hist_raw24_surface;
	/** Local Tone Mapping statistics buffer */
	struct stats_surface ltm_surface;

	/** Surfaces related configuration */
	struct {
		/** Input image surface width in pixels */
		uint16_t mr_width;
		/** Input image surface height in pixels */
		uint16_t mr_height;

		/** Height of slices used for processing the image */
		uint16_t slice_height;
		/** Width of first VI chunk in a line */
		uint16_t chunk_width_first;
		/**
		 * Width of VI chunks in the middle of a line, and/or width of
		 *  ISP tiles in middle of a slice
		 */
		uint16_t chunk_width_middle;
		/** Width of overfetch area in the beginning of VI chunks */
		uint16_t chunk_overfetch_width;
		/** Width of the leftmost ISP tile in a slice */
		uint16_t tile_width_first;
		/** Input image cfa */
		uint8_t mr_image_cfa;
		/** Reserved */
		uint8_t pad__;
		/** MR unit input image format value */
		uint32_t mr_image_def;
		/* TODO: should this be exposed to user mode? */
		/** MR unit input image format value */
		uint32_t mr_image_def1;
		/** SURFACE_CTL_MR register value */
		uint32_t surf_ctrl;
		/** Byte stride between start of lines. Must be ATOM aligned */
		uint32_t surf_stride_line;
		/** Byte stride between start of DPCM chunks. Must be ATOM aligned */
		uint32_t surf_stride_chunk;
	} surface_configs;

	/** Reserved */
	uint32_t pad2__;
	/** Base address of ISP PB2 memory */
	iova_t isp_pb2_mem;
	/* TODO: Isn't PB2 size constant, do we need this? */
	/** Size of the pushbuffer 2 memory */
	uint32_t isp_pb2_size;
	/** Reserved */
	uint32_t pad_pb__;

	/** Frame processing timeout in microseconds */
	uint32_t frame_timeout;

	/**
	 * Number of inputfences for given capture request.
	 * These fences are exclusively associated with ISP input ports and
	 * they support subframe sychronization.
	 */
	uint32_t num_inputfences;
	/** Progress syncpoint for each one of inputfences */
	struct syncpoint_info inputfences[ISP_MAX_INPUT_SURFACES];

	/* GID-STKHLDREQPLCL123-3812735 */
#define ISP_MAX_PREFENCES	MK_U32(14)

	/**
	 * Number of traditional prefences for given capture request.
	 * They are generic, so can be used for any pre-condition but do not
	 * support subframe synchronization
	 */
	uint32_t num_prefences;
	/** Reserved */
	uint32_t pad_prefences__;

	/** Syncpoint for each one of prefences */
	struct syncpoint_info prefences[ISP_MAX_PREFENCES];

	/** Engine result record – written by Falcon */
	struct engine_status_surface engine_status;

	/** Frame processing result record – written by RTCPU */
	struct capture_isp_status status;

	/** Offset for the next watermark within the watermark surface */
	struct watermark_mem_offset watermark_offset;

	/** Unique ID for ISP input buffer */
	uint64_t input_buffer_id;

	/* Information regarding the ISP program bound to this capture */
	uint32_t program_buffer_index;

	/** Reserved */
	uint32_t pad__[5];
} CAPTURE_DESCRIPTOR_ALIGN;

/**
 * @brief ISP capture descriptor memory information
 *
 * ISP capture descriptor memory information shared between
 * KMD and RCE only. This information cannot be part of
 * capture descriptor since it is shared with usermode
 * application.
 */
struct isp_capture_descriptor_memoryinfo {
	struct memoryinfo_surface input_mr_surfaces[ISP_MAX_INPUT_SURFACES];	// TODO RCE
	struct {
		struct memoryinfo_surface surfaces[ISP_MAX_OUTPUT_SURFACES];
	} outputs_mw[ISP_MAX_OUTPUTS];

	/** Flicker band (FB) statistics buffer */
	struct memoryinfo_surface fb_surface;
	/** Focus metrics (FM) statistics buffer */
	struct memoryinfo_surface fm_surface;
	/** Auto Focus Metrics (AFM) statistics buffer */
	struct memoryinfo_surface afm_surface; //
	/** Local Average Clipping (LAC0) unit 0 statistics buffer */
	struct memoryinfo_surface lac0_surface;
	/** Local Average Clipping (LAC1) unit 1 statistics buffer */
	struct memoryinfo_surface lac1_surface;
	/** Histogram (H0) unit 0 statistics buffer */
	struct memoryinfo_surface h0_surface;
	/** Histogram (H1) unit 1 statistics buffer */
	struct memoryinfo_surface h1_surface;
	/** Histogram (H2) unit 2 statistics buffer for ISP7 only*/
	struct memoryinfo_surface h2_surface;
	/** Pixel Replacement Unit (PRU) statistics buffer */
	struct memoryinfo_surface pru_bad_surface;
	/** Local Tone Mapping statistics buffer */
	struct memoryinfo_surface ltm_surface;
	/** RAW24 Histogram Unit statistics buffer */
	struct memoryinfo_surface hist_raw24_surface;
	/** Base address of ISP PB2 memory */
	struct memoryinfo_surface isp_pb2_mem;
	/** Engine result record – written by Falcon */
	struct memoryinfo_surface engine_status;
	/** Memory surface for watermark ring buffer written by ISP FW */
	struct memoryinfo_surface watermark_surface;
	/* Reserved */
	uint64_t reserved[2];
} CAPTURE_DESCRIPTOR_ALIGN;

/**
 * @brief PB2 size (ATOM aligned).
 *
 * NvCapture UMD makes sure to place PB2 just after above capture
 * descriptor buffer for each request, so that KMD and RCE can co-locate
 * PB2 and it's corresponding capture descriptor in memory.
 */
#define ISP_PB2_MAX_SIZE		MK_SIZE(512)

/**
 * @brief Size allocated for the ISP program push buffer
 */
#define NVISP5_ISP_PROGRAM_PB_SIZE	MK_SIZE(16384)

/**
* @brief Size allocated for the push buffer containing output & stats
* surface definitions. Final value TBD
*/
#define NVISP5_SURFACE_PB_SIZE		MK_SIZE(512)

/**
 * @Size of engine status surface used in both VI and ISP
 */
#define NV_ENGINE_STATUS_SURFACE_SIZE		MK_SIZE(16)

/**
 * @ brief Downscaler configuration information that is needed for building ISP config buffer.
 *
 * These registers cannot be included in push buffer but they must be provided in a structure
 * that RCE can parse. Format of the fields is same as in corresponding ISP registers.
 */
struct isp5_downscaler_configbuf {
	/**
	* Horizontal pixel increment, in U5.20 format. I.e. 2.5 means downscaling
	* by factor of 2.5. Corresponds to ISP_DM_H_PI register
	*/
	uint32_t pixel_incr_h;
	/**
	* Vertical pixel increment, in U5.20 format. I.e. 2.5 means downscaling
	* by factor of 2.5. Corresponds to ISP_DM_v_PI register
	*/
	uint32_t pixel_incr_v;

	/**
	* Offset of the first source image pixel to be used.
	* Topmost 16 bits - the leftmost column to be used
	* Lower 16 bits - the topmost line to be used
	*/
	uint32_t offset;

	/**
	* Size of the scaled destination image in pixels
	* Topmost 16 bits - height of destination image
	* Lowest 16 bits - Width of destination image
	*/
	uint32_t destsize;
};

/**
 * @brief ISP sub-units enabled bits.
 */
#define ISP5BLOCK_ENABLED_PRU_OUTLIER_REJECTION		MK_BIT32(0)
#define	ISP5BLOCK_ENABLED_PRU_STATS			MK_BIT32(1)
#define	ISP5BLOCK_ENABLED_PRU_HDR			MK_BIT32(2)
#define	ISP6BLOCK_ENABLED_PRU_RAW24_HIST		MK_BIT32(3) /* ISP6 */
#define	ISP5BLOCK_ENABLED_AP_DEMOSAIC			MK_BIT32(4)
#define	ISP5BLOCK_ENABLED_AP_CAR			MK_BIT32(5)
#define	ISP5BLOCK_ENABLED_AP_LTM_MODIFY			MK_BIT32(6)
#define	ISP5BLOCK_ENABLED_AP_LTM_STATS			MK_BIT32(7)
#define	ISP5BLOCK_ENABLED_AP_FOCUS_METRIC		MK_BIT32(8)
#define	ISP5BLOCK_ENABLED_FLICKERBAND			MK_BIT32(9)
#define	ISP5BLOCK_ENABLED_HISTOGRAM0			MK_BIT32(10)
#define	ISP5BLOCK_ENABLED_HISTOGRAM1			MK_BIT32(11)
#define	ISP5BLOCK_ENABLED_DOWNSCALER0_HOR		MK_BIT32(12)
#define	ISP5BLOCK_ENABLED_DOWNSCALER0_VERT		MK_BIT32(13)
#define	ISP5BLOCK_ENABLED_DOWNSCALER1_HOR		MK_BIT32(14)
#define	ISP5BLOCK_ENABLED_DOWNSCALER1_VERT		MK_BIT32(15)
#define	ISP5BLOCK_ENABLED_DOWNSCALER2_HOR		MK_BIT32(16)
#define	ISP5BLOCK_ENABLED_DOWNSCALER2_VERT		MK_BIT32(17)
#define	ISP5BLOCK_ENABLED_SHARPEN0			MK_BIT32(18)
#define	ISP5BLOCK_ENABLED_SHARPEN1			MK_BIT32(19)
#define	ISP5BLOCK_ENABLED_LAC0_REGION0			MK_BIT32(20)
#define	ISP5BLOCK_ENABLED_LAC0_REGION1			MK_BIT32(21)
#define	ISP5BLOCK_ENABLED_LAC0_REGION2			MK_BIT32(22)
#define	ISP5BLOCK_ENABLED_LAC0_REGION3			MK_BIT32(23)
#define	ISP5BLOCK_ENABLED_LAC1_REGION0			MK_BIT32(24)
#define	ISP5BLOCK_ENABLED_LAC1_REGION1			MK_BIT32(25)
#define	ISP5BLOCK_ENABLED_LAC1_REGION2			MK_BIT32(26)
#define	ISP5BLOCK_ENABLED_LAC1_REGION3			MK_BIT32(27)
/**
 * Enable ISP6 LTM Softkey automatic update feature
 */
#define ISP6BLOCK_ENABLED_AP_LTM_SK_UPDATE		MK_BIT32(28)
/**
 * Enable ISP7 HIST2
 */
#define	ISP7BLOCK_ENABLED_HISTOGRAM2			MK_BIT32(29)
/**
 * @brief ISP overfetch requirements.
 *
 * ISP kernel needs access to pixels outside the active area of a tile
 * to ensure continuous processing across tile borders. The amount of data
 * needed depends on features enabled and some ISP parameters so this
 * is program dependent.
 *
 * ISP extrapolates values outside image borders, so overfetch is needed only
 * for borders between tiles.
 */

struct isp_overfetch {
	/** Number of pixels needed from the left side of tile */
	uint8_t left;
	/** Number of pixels needed from the right side of tile */
	uint8_t right;
	/** Number of pixels needed from above the tile */
	uint8_t top;
	/** Number of pixels needed from below the tile */
	uint8_t bottom;
	/**
	 * Number of pixels needed by PRU unit from left and right sides of the tile.
	 * This is needed to adjust tile border locations so that they align correctly
	 * at demosaic input.
	 */
	uint8_t pru_ovf_h;
	/**
	 * Alignment requirement for tile width. Minimum alignment is 2 pixels, but
	 * if CAR is used this must be set to half of LPF kernel width.
	 */
	uint8_t alignment;
	/** Reserved */
	uint8_t pad1__[2];
};

/**
 * @brief Identifier for ISP5
 */
#define ISP_TYPE_ID_ISP5 MK_U16(3)

/**
 * @brief Identifier for ISP6
 */
#define ISP_TYPE_ID_ISP6 MK_U16(4)

/**
 * @brief Identifier for ISP7
 */
#define ISP_TYPE_ID_ISP7 MK_U16(5)

/**
 * @brief Magic bytes to detect ISP program struct with version information
 */

#define ISP5_PROGRAM_STRUCT_ID MK_U32(0x50505349)

/**
 * @brief Version of ISP program struct layout
 *
 * Value of this constant must be increased every time when the memory layout of
 * isp5_program struct changes.
 */
#define ISP5_PROGRAM_STRUCT_VERSION MK_U16(3)


/**
 * @brief ISP program buffer
 *
 * Settings needed by RCE ISP driver to generate config buffer.
 * Content and format of these fields is the same as corresponding ISP config buffer fields.
 * See ISP_Microcode.docx for detailed description.
 */
struct isp5_program {
	/**
	 * @brief "Magic bytes" to identify memory area as an ISP program
	 */
	uint32_t isp_program_struct_id;

	/**
	 * @brief Version of the ISP program structure
	 */
	uint16_t isp_program_struct_version;

	/**
	 * @brief Target ISP for the ISP program.
	 */
	uint16_t isp_type;

	/**
	 * Sources for LS, AP and PRU blocks.
	 * Format is same as in ISP's XB_SRC_0 register
	 * For ISP7, this is the stats crossbar used for
	 * HIST1 and LAC1, with ISP_STATS_XB_SRC register format.
	 */
	uint32_t xbsrc0;

	/**
	 * Sources for AT[0-2] and TF[0-1] blocks
	 * Format is same as in ISP's XB_SRC_1 register
	 * For ISP7, this is the output crossbar used for
	 * TF0, DS[0-2], with ISP_OUT_XB_SRC register format.
	 */
	uint32_t xbsrc1;

	/**
	 * Sources for DS[0-2] and MW[0-2] blocks
	 * Format is same as in ISP's XB_SRC_2 register
	 * For ISP7, this is the AT1 MuxSelect used for
	 * AT1, with ISP_AT1_XB_SRC register format.
	 */
	uint32_t xbsrc2;

	/**
	 * Sources for FB, LAC[0-1] and HIST[0-1] blocks
	 * Format is same as in ISP's XB_SRC_3 register
	 * For ISP7, this is not used.
	 */
	uint32_t xbsrc3;

	/**
	 * Bitmask to describe which of ISP blocks are enabled.
	 * See microcode documentation for details.
	 */
	uint32_t enables_config;

	/**
	 * AFM configuration. See microcode documentation for details.
	 */
	uint32_t afm_ctrl;

	/**
	 * Mask for stats blocks enabled.
	 */
	uint32_t stats_aidx_flag;

	/**
	 * Size used for the push buffer in 4-byte words.
	 */
	uint32_t pushbuffer_size;

	/**
	 * Horizontal pixel increment for downscalers, in
	 * U5.20 format. I.e. 2.5 means downscaling
	 * by factor of 2.5. Corresponds to ISP_DM_H_PI register.
	 * This is needed by ISP Falcon firmware to program
	 * tile starting state correctly.
	 */
	uint32_t ds0_pixel_incr_h;
	uint32_t ds1_pixel_incr_h;
	uint32_t ds2_pixel_incr_h;

	/** ISP overfetch requirements */
	struct isp_overfetch overfetch;

	/** memory write crop region info*/
	struct {
		struct isp_crop_rect mw_crop;
	} outputs_mw[ISP_MAX_OUTPUTS];

	/** Reserved */
	uint32_t pad1__[11];

	/**
	 * Push buffer containing ISP settings related to this program.
	 * No relocations will be done for this push buffer; all registers
	 * that contain memory addresses that require relocation must be
	 * specified in the capture descriptor ISP payload.
	 */
	uint32_t pushbuffer[NVISP5_ISP_PROGRAM_PB_SIZE / sizeof(uint32_t)]
			CAPTURE_DESCRIPTOR_ALIGN;

} CAPTURE_DESCRIPTOR_ALIGN;

/**
 * @brief ISP Program ringbuffer element
 *
 * Each element in the ISP program ringbuffer contains a program descriptor immediately followed
 * isp program.
 */
struct isp5_program_entry {
	/** ISP capture descriptor */
	struct isp_program_descriptor prog_desc;
	/** ISP program buffer */
	struct isp5_program isp_prog;
} CAPTURE_DESCRIPTOR_ALIGN;

#pragma GCC diagnostic ignored "-Wpadded"

#endif /* INCLUDE_CAMRTC_CAPTURE_H */
