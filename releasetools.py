# Copyright (C) 2009 The Android Open Source Project
# Copyright (C) 2019 The LineageOS Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import common
import re

def FullOTA_InstallEnd(info):
  OTA_InstallEnd(info, False)
  return

def IncrementalOTA_InstallEnd(info):
  OTA_InstallEnd(info, True)
  return

def AddImage(info, basename, dest, incremental):
  name = basename
  if incremental:
    input_zip = info.source_zip
  else:
    input_zip = info.input_zip
  data = input_zip.read("IMAGES/" + basename)
  common.ZipWriteStr(info.output_zip, name, data)
  info.script.AppendExtra('package_extract_file("%s", "%s");' % (name, dest))

def OTA_InstallEnd(info, incremental):
  info.script.Print("Patching firmware images...")
  AddImage(info, "dtbo.img", "/dev/block/bootdevice/by-name/dtbo", incremental)
  return
