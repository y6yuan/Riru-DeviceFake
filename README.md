# Riru - Device Fake

A module of [Riru](https://github.com/RikkaApps/Riru). Fake device by changing system properties and inject builds in related packages.

## What does this module do

By default, `__system_property_get` and `__system_property_read_callback` (API 26+) will be hooked, and android.os.Build will be injected in these packages

* com.huawei.hwid


and the return value will be changed

* `ro.product.brand` -> `HUAWEI`
* `ro.product.manufacturer` -> `HUAWEI`
* `ro.product.model` -> `NOH-AN00`

and the value of android.os.Build will be injected

* `BRAND` -> `HUAWEI`
* `MANUFACTURER` -> `HUAWEI`
* `MODEL` -> `NOH-AN00`


## Configuration

Note, reboot (or kill zygote) is required to let the new settings take effect.

### Packages

`/data/adb/riru/modules/device_fake/config/packages/<package name>`

### Properties

`/data/adb/riru/modules/device_fake/config/properties/<key>` (file content is value)

### Builds

`/data/adb/riru/modules/device_fake/config/builds/<key>` (file content is value)

### Credits
- [RikkaW](https://github.com/RikkaApps) - Creator of Riru Magisk module, which provides a way to inject codes into zygote process
- [topjohnwu](https://github.com/topjohnwu) - Creator of Magisk
- [MiPushFrameWork](https://github.com/MiPushFramework) - MiPushFakeForRiru(https://github.com/MiPushFramework/MiPushFakeForRiru)
