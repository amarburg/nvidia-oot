/*
 * Copyright (c) 2019-2020, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <dce.h>
#include <dce-util-common.h>

/**
 * dce_driver_init - Initializes the various sw components
 *					and few hw elements dce.
 *
 * @d : Pointer to tegra_dce struct.
 *
 * Return : 0 if successful.
 */
int dce_driver_init(struct tegra_dce *d)
{
	int ret = 0;

	ret = dce_boot_interface_init(d);
	if (ret) {
		dce_err(d, "dce boot interface init failed");
		goto err_boot_interface_init;
	}

	ret = dce_admin_init(d);
	if (ret) {
		dce_err(d, "dce admin interface init failed");
		goto err_admin_interface_init;
	}

	ret = dce_worker_thread_init(d);
	if (ret) {
		dce_err(d, "dce worker thread init failed");
		goto err_worker_thread_init;
	}

	return ret;

err_worker_thread_init:
	dce_admin_deinit(d);
err_admin_interface_init:
	dce_boot_interface_deinit(d);
err_boot_interface_init:
	d->boot_status |= DCE_STATUS_FAILED;
	return ret;
}

/**
 * dce_driver_deinit - Release various sw resources
 *					associated with dce.
 *
 * @d : Pointer to tegra_dce struct.
 *
 * Return : Void
 */

void dce_driver_deinit(struct tegra_dce *d)
{
	/*  TODO : Reset DCE ? */
	dce_worker_thread_deinit(d);

	dce_admin_deinit(d);

	dce_boot_interface_deinit(d);

	dce_release_fw(d, d->fw_data);
}
