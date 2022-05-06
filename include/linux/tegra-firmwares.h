// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2022, NVIDIA CORPORATION, All rights reserved.

#ifndef __TEGRA_FIRMWARES_H
#define __TEGRA_FIRMWARES_H

/*
 * max size of version string
 */
#define TFW_VERSION_MAX_SIZE 256

struct device *tegrafw_register(const char *name,
			const u32 flags,
			ssize_t (*reader)(struct device *dev, char *, size_t),
			const char *string);
void tegrafw_unregister(struct device *fwdev);
struct device *devm_tegrafw_register(struct device *dev,
			const char *name,
			const u32 flags,
			ssize_t (*reader)(struct device *dev, char *, size_t),
			const char *string);
void devm_tegrafw_unregister(struct device *dev, struct device *fwdev);
void tegrafw_invalidate(struct device *fwdev);
struct device *devm_tegrafw_register_dt_string(struct device *dev,
						const char *name,
						const char *path,
						const char *property);
enum {
	TFW_NORMAL	= 0x0000,
	TFW_DONT_CACHE	= 0x0001,
	TFW_MAX		= 0xFFFF,
};

static inline struct device *tegrafw_register_string(const char *name,
						     const char *string)
{
	return tegrafw_register(name, TFW_NORMAL, NULL, string);
}

static inline struct device *devm_tegrafw_register_string(struct device *dev,
						     const char *name,
						     const char *string)
{
	return devm_tegrafw_register(dev, name, TFW_NORMAL, NULL, string);
}

static inline struct device *tegrafw_register_dt_string(const char *name,
							const char *path,
							const char *property)
{
	return devm_tegrafw_register_dt_string(NULL, name, path, property);
}

#endif /* __TEGRA_FIRMWARES_CLASS */
