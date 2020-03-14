# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    aaudio.hw_burst_min_usec=2000 \
    aaudio.mmap_exclusive_policy=2 \
    aaudio.mmap_policy=2 \
    af.fast_track_multiplier=1 \
    audio.deep_buffer.media=true \
    audio.offload.video=true \
    persist.vendor.audio.fluence.speaker=true \
    persist.vendor.audio.fluence.tmic.enabled=false \
    persist.vendor.audio.fluence.voicecall=true \
    persist.vendor.audio.fluence.voicecomm=true \
    persist.vendor.audio.fluence.voicerec=false \
    persist.vendor.audio.ras.enabled=false \
    persist.vendor.audio.voicecall.speaker.stereo=true \
    ro.af.client_heap_size_kbyte=7168 \
    ro.config.media_vol_steps=25 \
    ro.config.vc_call_vol_steps=7 \
    ro.vendor.audio.sdk.fluencetype=fluence \
    ro.vendor.audio.sdk.ssr=false \
    ro.vendor.audio.sfx.earadj=true \
    ro.vendor.audio.soundfx.type=mi \
    ro.vendor.audio.soundfx.usb=true \
    ro.vendor.audio.soundtrigger.cnn.adsp.level=27 \
    ro.vendor.audio.soundtrigger.cnn.level=27 \
    ro.vendor.audio.soundtrigger.gmm.adsp.level=50 \
    ro.vendor.audio.soundtrigger.gmm.level=50 \
    ro.vendor.audio.soundtrigger.gmm.user.adsp.level=10 \
    ro.vendor.audio.soundtrigger.gmm.user.level=10 \
    ro.vendor.audio.soundtrigger.lowpower=true \
    ro.vendor.audio.soundtrigger.training.level=50 \
    ro.vendor.audio.soundtrigger.vop.adsp.level=10 \
    ro.vendor.audio.soundtrigger.vop.level=10 \
    ro.vendor.audio.soundtrigger=sva \
    vendor.audio.adm.buffering.ms=16 \
    vendor.audio.feature.a2dp_offload.enable=true \
    vendor.audio.feature.afe_proxy.enable=true \
    vendor.audio.feature.anc_headset.enable=true \
    vendor.audio.feature.audiozoom.enable=false \
    vendor.audio.feature.battery_listener.enable=true \
    vendor.audio.feature.compr_cap.enable=false \
    vendor.audio.feature.compr_voip.enable=false \
    vendor.audio.feature.compress_in.enable=false \
    vendor.audio.feature.compress_meta_data.enable=true \
    vendor.audio.feature.concurrent_capture.enable=false \
    vendor.audio.feature.custom_stereo.enable=true \
    vendor.audio.feature.deepbuffer_as_primary.enable=false \
    vendor.audio.feature.display_port.enable=true \
    vendor.audio.feature.dsm_feedback.enable=false \
    vendor.audio.feature.dynamic_ecns.enable=false \
    vendor.audio.feature.ext_hw_plugin.enable=true \
    vendor.audio.feature.external_dsp.enable=false \
    vendor.audio.feature.external_speaker.enable=false \
    vendor.audio.feature.external_speaker_tfa.enable=false \
    vendor.audio.feature.fluence.enable=true \
    vendor.audio.feature.fm.enable=true \
    vendor.audio.feature.hdmi_edid.enable=true \
    vendor.audio.feature.hdmi_passthrough.enable=true \
    vendor.audio.feature.hfp.enable=true \
    vendor.audio.feature.hifi_audio.enable=false \
    vendor.audio.feature.hwdep_cal.enable=false \
    vendor.audio.feature.incall_music.enable=true \
    vendor.audio.feature.keep_alive.enable=true \
    vendor.audio.feature.kpi_optimize.enable=true \
    vendor.audio.feature.maxx_audio.enable=false \
    vendor.audio.feature.multi_voice_session.enable=true \
    vendor.audio.feature.ras.enable=true \
    vendor.audio.feature.record_play_concurency.enable=false \
    vendor.audio.feature.snd_mon.enable=true \
    vendor.audio.feature.spkr_prot.enable=true \
    vendor.audio.feature.src_trkn.enable=true \
    vendor.audio.feature.ssrec.enable=true \
    vendor.audio.feature.usb_offload.enable=true \
    vendor.audio.feature.usb_offload_burst_mode.enable=true \
    vendor.audio.feature.usb_offload_sidetone_volume.enable=false \
    vendor.audio.feature.vbat.enable=true \
    vendor.audio.feature.wsa.enable=false \
    vendor.audio.flac.sw.decoder.24bit=true \
    vendor.audio.hal.output.suspend.supported=false \
    vendor.audio.hw.aac.encoder=false \
    vendor.audio.offload.buffer.size.kb=32 \
    vendor.audio.offload.gapless.enabled=true \
    vendor.audio.offload.multiaac.enable=true \
    vendor.audio.offload.multiple.enabled=false \
    vendor.audio.offload.passthrough=false \
    vendor.audio.offload.track.enable=false \
    vendor.audio.parser.ip.buffer.size=262144 \
    vendor.audio.safx.pbe.enabled=false \
    vendor.audio.tunnel.encode=false \
    vendor.audio.usb.disable.sidetone=true \
    vendor.audio.use.sw.alac.decoder=true \
    vendor.audio.use.sw.ape.decoder=true \
    vendor.audio.volume.headset.gain.depcal=true \
    vendor.audio_hal.in_period_size=144 \
    vendor.audio_hal.period_multiplier=3 \
    vendor.audio_hal.period_size=192

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    persist.bluetooth.a2dp_offload.cap=sbc-aac-aptx-aptxhd-ldac \
    persist.bluetooth.a2dp_offload.disabled=false \
    persist.bt.enableAptXHD=true \
    persist.service.btui.use_aptx=1 \
    persist.vendor.bt.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aptxadaptive-aac-ldac \
    persist.vendor.bt.aac_frm_ctl.enabled=false \
    persist.vendor.btstack.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aptxadaptive-aac-ldac \
    persist.vendor.btstack.connect.peer_earbud=true \
    persist.vendor.btstack.enable.twsplus=true \
    persist.vendor.btstack.enable.twsplussho=true \
    persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptive \
    persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true \
    persist.vendor.qcom.bluetooth.enable.splita2dp=true \
    persist.vendor.qcom.bluetooth.twsp_state.enabled=false \
    ro.bluetooth.a2dp_offload.supported=true \
    vendor.bluetooth.soc=cherokee \
    vendor.qcom.bluetooth.soc=cherokee

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
    persist.camera.sat.fallback.dist.d=10 \
    persist.camera.sat.fallback.dist=40 \
    persist.camera.sat.fallback.lux.d=50 \
    persist.camera.sat.fallback.luxindex=360 \
    persist.vendor.camera.enableNCSService=TRUE \
    persist.vendor.camera.enableTOFInterface=TRUE \
    persist.vendor.camera.mfnrframenum=8 \
    persist.vendor.camera.multicam=TRUE \
    persist.camera.gyro.disable=0 \
    camera.disable_zsl_mode=true \
    vendor.camera.aux.packagelist=org.codeaurora.snapcam,com.android.camera,org.lineageos.snap

# CNE and DPM
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.cne.feature=1 \
    persist.vendor.dpm.feature=1 \
    persist.vendor.dpm.loglevel=0 \
    persist.vendor.dpm.nsrm.bkg.evt=3955

# Data modules
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.data.iwlan.enable=true \
    persist.vendor.data.mode=concurrent

# Display
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.display.cabl=0 \
    ro.vendor.display.ad=1 \
    ro.vendor.display.ad.hdr_calib_data=/vendor/etc/hdr_config.cfg \
    ro.vendor.display.ad.sdr_calib_data=/vendor/etc/sdr_config.cfg \
    ro.vendor.display.sensortype=2 \
    ro.sf.lcd_density=440 \
    vendor.display.comp_mask=0 \
    vendor.display.dataspace_saturation_matrix=1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0 \
    vendor.display.disable_decimation=1 \
    vendor.display.disable_excl_rect=0 \
    vendor.display.disable_hw_recovery_dump=1 \
    vendor.display.disable_inline_rotator=1 \
    vendor.display.disable_scaler=0 \
    vendor.display.disable_ui_3d_tonemap=1 \
    vendor.display.enable_default_color_mode=1 \
    vendor.display.enable_null_display=0 \
    vendor.display.enable_optimize_refresh=1

# FRP
PRODUCT_PROPERTY_OVERRIDES += \
    ro.frp.pst=/dev/block/bootdevice/by-name/frp

# Gatekeeper
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.gatekeeper.disable_spu=true

# Graphics
PRODUCT_PROPERTY_OVERRIDES += \
    debug.egl.hw=0 \
    debug.mdpcomp.logs=0 \
    debug.sf.enable_hwc_vds=1 \
    debug.sf.hw=1 \
    debug.sf.latch_unsignaled=1 \
    vendor.gralloc.disable_ubwc=0 \
    ro.opengles.version=196610 \
    ro.hardware.egl=adreno \
    ro.surface_flinger.has_HDR_display=true \
    ro.surface_flinger.has_wide_color_display=true \
    ro.hardware.vulkan=adreno \
    vendor.gralloc.disable_ubwc=0

# IMS
PRODUCT_PROPERTY_OVERRIDES += \
    persist.dbg.volte_avail_ovr=1 \
    persist.dbg.vt_avail_ovr=1  \
    persist.dbg.wfc_avail_ovr=1

# Media
PRODUCT_PROPERTY_OVERRIDES += \
    media.aac_51_output_enabled=true \
    media.stagefright.enable-aac=true \
    media.stagefright.enable-fma2dp=true \
    media.stagefright.enable-http=true \
    media.stagefright.enable-player=true \
    media.stagefright.enable-qcp=true \
    media.stagefright.enable-scan=true \
    media.stagefright.thumbnail.prefer_hw_codecs=true \
    mmp.enable.3g2=true \
    media.settings.xml=/vendor/etc/media_profiles_vendor.xml \
    persist.mm.enable.prefetch=true

# Memory
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.qti.sys.fw.bservice_enable=true \
    ro.vendor.qti.va_aosp.support=1 \
    ro.vendor.qti.va_odm.support=1

# Netflix custom property
PRODUCT_PROPERTY_OVERRIDES += \
    ro.netflix.bsp_rev=Q855-16947-1

# NFC DT
PRODUCT_PROPERTY_OVERRIDES += \
    ro.nfc.port=I2C

# Property to enable/disable PASR
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.power.pasr.enabled=true

# Perf
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=libqti-perfd-client.so

# RCS
PRODUCT_PROPERTY_OVERRIDES += \
    persist.rcs.supported=1

# RIL
PRODUCT_PROPERTY_OVERRIDES += \
    DEVICE_PROVISIONED=1 \
    persist.radio.apm_sim_not_pwdn=1 \
    persist.radio.dynamic_sar=false \
    persist.radio.multisim.config=dsds \
    persist.radio.NO_STAPA=1 \
    persist.radio.sar_sar_sensor=true \
    persist.radio.VT_HYBRID_ENABLE=1 \
    persist.sys.fflag.override.settings_network_and_internet_v2=true \
    persist.vendor.radio.apm_sim_not_pwdn=1 \
    persist.vendor.radio.custom_ecc=1 \
    persist.vendor.radio.enable_temp_dds=true \
    persist.vendor.radio.enableadvancedscan=true \
    persist.vendor.radio.force_on_dc=true \
    persist.vendor.radio.procedure_bytes=SKIP \
    persist.vendor.radio.rat_on=combine \
    persist.vendor.radio.redir_party_num=1 \
    persist.vendor.radio.report_codec=1 \
    persist.vendor.radio.sib16_support=1 \
    ril.subscription.types=RUIM \
    ro.telephony.default_cdma_sub=0 \
    ro.telephony.default_network=22,22 \
    telephony.lteOnCdmaDevice=1

# Sensors
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.sensors.enable.mag_filter=true \
    persist.vendor.uspsensor=default

# SSR
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.ssr.restart_level=ALL_ENABLE

# SurfaceFlinger
PRODUCT_PROPERTY_OVERRIDES += \
    debug.sf.enable_gl_backpressure=1 \
    debug.sf.early_phase_offset_ns=500000 \
    debug.sf.early_app_phase_offset_ns=500000 \
    debug.sf.early_gl_phase_offset_ns=3000000 \
    debug.sf.early_gl_app_phase_offset_ns=15000000 \
    ro.surface_flinger.max_frame_buffer_acquired_buffers=3 \
    ro.surface_flinger.vsync_event_phase_offset_ns=2000000 \
    ro.surface_flinger.vsync_sf_event_phase_offset_ns=6000000

# WFD
PRODUCT_PROPERTY_OVERRIDES += \
    persist.debug.wfd.enable=1 \
    persist.sys.wfd.virtual=0

# WiFi
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0
