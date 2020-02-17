# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/markaligbe/Documents/PlatformIO/Projects/leviathans_breath_pio/gui/gui.ui'
#
# Created by: PyQt5 UI code generator 5.14.1
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_mw_main(object):
    def setupUi(self, mw_main):
        mw_main.setObjectName("mw_main")
        mw_main.resize(1024, 618)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("/home/markaligbe/Documents/PlatformIO/Projects/leviathans_breath_pio/gui/logo.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        mw_main.setWindowIcon(icon)
        self.centralwidget = QtWidgets.QWidget(mw_main)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayout_2 = QtWidgets.QGridLayout(self.centralwidget)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.sp_main = QtWidgets.QSplitter(self.centralwidget)
        self.sp_main.setOrientation(QtCore.Qt.Horizontal)
        self.sp_main.setObjectName("sp_main")
        self.sa_chart = QtWidgets.QScrollArea(self.sp_main)
        self.sa_chart.setWidgetResizable(True)
        self.sa_chart.setObjectName("sa_chart")
        self.sawc_chart = QtWidgets.QWidget()
        self.sawc_chart.setGeometry(QtCore.QRect(0, 0, 708, 797))
        self.sawc_chart.setObjectName("sawc_chart")
        self.gl_sawc_chart = QtWidgets.QGridLayout(self.sawc_chart)
        self.gl_sawc_chart.setObjectName("gl_sawc_chart")
        self.gb_fan_curve_editor = QtWidgets.QGroupBox(self.sawc_chart)
        self.gb_fan_curve_editor.setCheckable(True)
        self.gb_fan_curve_editor.setObjectName("gb_fan_curve_editor")
        self.gl_fan_curve_editor = QtWidgets.QGridLayout(self.gb_fan_curve_editor)
        self.gl_fan_curve_editor.setObjectName("gl_fan_curve_editor")
        self.f_fan_curve_editor_anim = QtWidgets.QFrame(self.gb_fan_curve_editor)
        self.f_fan_curve_editor_anim.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_fan_curve_editor_anim.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_fan_curve_editor_anim.setObjectName("f_fan_curve_editor_anim")
        self.gl_fan_curve_editor_anim = QtWidgets.QGridLayout(self.f_fan_curve_editor_anim)
        self.gl_fan_curve_editor_anim.setContentsMargins(0, 0, 0, 0)
        self.gl_fan_curve_editor_anim.setSpacing(0)
        self.gl_fan_curve_editor_anim.setObjectName("gl_fan_curve_editor_anim")
        self.gb_fan_curve_options = QtWidgets.QGroupBox(self.f_fan_curve_editor_anim)
        self.gb_fan_curve_options.setCheckable(True)
        self.gb_fan_curve_options.setObjectName("gb_fan_curve_options")
        self.formLayout = QtWidgets.QFormLayout(self.gb_fan_curve_options)
        self.formLayout.setObjectName("formLayout")
        self.f_fan_curve_options_anim = QtWidgets.QFrame(self.gb_fan_curve_options)
        self.f_fan_curve_options_anim.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_fan_curve_options_anim.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_fan_curve_options_anim.setObjectName("f_fan_curve_options_anim")
        self.gl_fan_curve_options_anim = QtWidgets.QGridLayout(self.f_fan_curve_options_anim)
        self.gl_fan_curve_options_anim.setContentsMargins(0, 0, 0, 0)
        self.gl_fan_curve_options_anim.setSpacing(0)
        self.gl_fan_curve_options_anim.setObjectName("gl_fan_curve_options_anim")
        self.f_fan_options_preset = QtWidgets.QFrame(self.f_fan_curve_options_anim)
        self.f_fan_options_preset.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_fan_options_preset.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_fan_options_preset.setObjectName("f_fan_options_preset")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.f_fan_options_preset)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label_6 = QtWidgets.QLabel(self.f_fan_options_preset)
        self.label_6.setObjectName("label_6")
        self.horizontalLayout.addWidget(self.label_6)
        self.cb_fan_options_preset = QtWidgets.QComboBox(self.f_fan_options_preset)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.cb_fan_options_preset.sizePolicy().hasHeightForWidth())
        self.cb_fan_options_preset.setSizePolicy(sizePolicy)
        self.cb_fan_options_preset.setEditable(True)
        self.cb_fan_options_preset.setInsertPolicy(QtWidgets.QComboBox.InsertAlphabetically)
        self.cb_fan_options_preset.setObjectName("cb_fan_options_preset")
        self.horizontalLayout.addWidget(self.cb_fan_options_preset)
        self.pb_fan_options_save = QtWidgets.QPushButton(self.f_fan_options_preset)
        self.pb_fan_options_save.setObjectName("pb_fan_options_save")
        self.horizontalLayout.addWidget(self.pb_fan_options_save)
        self.pb_fan_options_load = QtWidgets.QPushButton(self.f_fan_options_preset)
        self.pb_fan_options_load.setObjectName("pb_fan_options_load")
        self.horizontalLayout.addWidget(self.pb_fan_options_load)
        self.gl_fan_curve_options_anim.addWidget(self.f_fan_options_preset, 0, 0, 1, 1)
        self.f_fan_curve_temperature_source = QtWidgets.QFrame(self.f_fan_curve_options_anim)
        self.f_fan_curve_temperature_source.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_fan_curve_temperature_source.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_fan_curve_temperature_source.setObjectName("f_fan_curve_temperature_source")
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout(self.f_fan_curve_temperature_source)
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.label_4 = QtWidgets.QLabel(self.f_fan_curve_temperature_source)
        self.label_4.setObjectName("label_4")
        self.horizontalLayout_3.addWidget(self.label_4)
        self.cb_fan_curve_temperature_source_selection = QtWidgets.QComboBox(self.f_fan_curve_temperature_source)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.cb_fan_curve_temperature_source_selection.sizePolicy().hasHeightForWidth())
        self.cb_fan_curve_temperature_source_selection.setSizePolicy(sizePolicy)
        self.cb_fan_curve_temperature_source_selection.setEditable(True)
        self.cb_fan_curve_temperature_source_selection.setInsertPolicy(QtWidgets.QComboBox.NoInsert)
        self.cb_fan_curve_temperature_source_selection.setObjectName("cb_fan_curve_temperature_source_selection")
        self.horizontalLayout_3.addWidget(self.cb_fan_curve_temperature_source_selection)
        self.gl_fan_curve_options_anim.addWidget(self.f_fan_curve_temperature_source, 1, 0, 1, 1)
        self.f_pwm_controlled = QtWidgets.QFrame(self.f_fan_curve_options_anim)
        self.f_pwm_controlled.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_pwm_controlled.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_pwm_controlled.setObjectName("f_pwm_controlled")
        self.formLayout_4 = QtWidgets.QFormLayout(self.f_pwm_controlled)
        self.formLayout_4.setObjectName("formLayout_4")
        self.cb_pwm_controlled = QtWidgets.QCheckBox(self.f_pwm_controlled)
        self.cb_pwm_controlled.setObjectName("cb_pwm_controlled")
        self.formLayout_4.setWidget(0, QtWidgets.QFormLayout.LabelRole, self.cb_pwm_controlled)
        self.gl_fan_curve_options_anim.addWidget(self.f_pwm_controlled, 2, 0, 1, 1)
        self.formLayout.setWidget(0, QtWidgets.QFormLayout.SpanningRole, self.f_fan_curve_options_anim)
        self.gl_fan_curve_editor_anim.addWidget(self.gb_fan_curve_options, 2, 0, 1, 1)
        self.f_fan_curve_ph = QtWidgets.QFrame(self.f_fan_curve_editor_anim)
        self.f_fan_curve_ph.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_fan_curve_ph.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_fan_curve_ph.setObjectName("f_fan_curve_ph")
        self.gl_fan_curve_ph = QtWidgets.QGridLayout(self.f_fan_curve_ph)
        self.gl_fan_curve_ph.setContentsMargins(0, 0, 0, 0)
        self.gl_fan_curve_ph.setSpacing(0)
        self.gl_fan_curve_ph.setObjectName("gl_fan_curve_ph")
        self.gl_fan_curve_editor_anim.addWidget(self.f_fan_curve_ph, 1, 0, 1, 1)
        self.cb_fan_curve_selection = QtWidgets.QComboBox(self.f_fan_curve_editor_anim)
        self.cb_fan_curve_selection.setEditable(True)
        self.cb_fan_curve_selection.setInsertPolicy(QtWidgets.QComboBox.NoInsert)
        self.cb_fan_curve_selection.setObjectName("cb_fan_curve_selection")
        self.gl_fan_curve_editor_anim.addWidget(self.cb_fan_curve_selection, 0, 0, 1, 1)
        self.gl_fan_curve_editor.addWidget(self.f_fan_curve_editor_anim, 0, 0, 1, 1)
        self.gl_sawc_chart.addWidget(self.gb_fan_curve_editor, 1, 0, 1, 1)
        self.gb_temperature_display = QtWidgets.QGroupBox(self.sawc_chart)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.MinimumExpanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.gb_temperature_display.sizePolicy().hasHeightForWidth())
        self.gb_temperature_display.setSizePolicy(sizePolicy)
        self.gb_temperature_display.setCheckable(True)
        self.gb_temperature_display.setObjectName("gb_temperature_display")
        self.gridLayout_6 = QtWidgets.QGridLayout(self.gb_temperature_display)
        self.gridLayout_6.setObjectName("gridLayout_6")
        self.f_temperature_display_anim = QtWidgets.QFrame(self.gb_temperature_display)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.MinimumExpanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.f_temperature_display_anim.sizePolicy().hasHeightForWidth())
        self.f_temperature_display_anim.setSizePolicy(sizePolicy)
        self.f_temperature_display_anim.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_temperature_display_anim.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_temperature_display_anim.setObjectName("f_temperature_display_anim")
        self.vl_temperature_display_anim = QtWidgets.QVBoxLayout(self.f_temperature_display_anim)
        self.vl_temperature_display_anim.setContentsMargins(0, 0, 0, 0)
        self.vl_temperature_display_anim.setSpacing(0)
        self.vl_temperature_display_anim.setObjectName("vl_temperature_display_anim")
        self.cb_temperature_display_selection = QtWidgets.QComboBox(self.f_temperature_display_anim)
        self.cb_temperature_display_selection.setEditable(True)
        self.cb_temperature_display_selection.setInsertPolicy(QtWidgets.QComboBox.NoInsert)
        self.cb_temperature_display_selection.setObjectName("cb_temperature_display_selection")
        self.vl_temperature_display_anim.addWidget(self.cb_temperature_display_selection)
        self.f_temperature_display_ph = QtWidgets.QFrame(self.f_temperature_display_anim)
        self.f_temperature_display_ph.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_temperature_display_ph.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_temperature_display_ph.setObjectName("f_temperature_display_ph")
        self.gl_temperature_display_ph = QtWidgets.QGridLayout(self.f_temperature_display_ph)
        self.gl_temperature_display_ph.setContentsMargins(0, 0, 0, 0)
        self.gl_temperature_display_ph.setSpacing(0)
        self.gl_temperature_display_ph.setObjectName("gl_temperature_display_ph")
        self.vl_temperature_display_anim.addWidget(self.f_temperature_display_ph)
        self.gridLayout_6.addWidget(self.f_temperature_display_anim, 0, 0, 1, 1)
        self.gl_sawc_chart.addWidget(self.gb_temperature_display, 0, 0, 1, 1)
        self.gb_led_curve_editor = QtWidgets.QGroupBox(self.sawc_chart)
        self.gb_led_curve_editor.setCheckable(True)
        self.gb_led_curve_editor.setObjectName("gb_led_curve_editor")
        self.gridLayout_14 = QtWidgets.QGridLayout(self.gb_led_curve_editor)
        self.gridLayout_14.setObjectName("gridLayout_14")
        self.f_led_curve_editor_anim = QtWidgets.QFrame(self.gb_led_curve_editor)
        self.f_led_curve_editor_anim.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_led_curve_editor_anim.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_led_curve_editor_anim.setObjectName("f_led_curve_editor_anim")
        self.gl_led_curve_editor_anim = QtWidgets.QGridLayout(self.f_led_curve_editor_anim)
        self.gl_led_curve_editor_anim.setContentsMargins(0, 0, 0, 0)
        self.gl_led_curve_editor_anim.setSpacing(0)
        self.gl_led_curve_editor_anim.setObjectName("gl_led_curve_editor_anim")
        self.tw_led_curve_channel = QtWidgets.QTabWidget(self.f_led_curve_editor_anim)
        self.tw_led_curve_channel.setObjectName("tw_led_curve_channel")
        self.tw_led_channel_t_r = QtWidgets.QWidget()
        self.tw_led_channel_t_r.setObjectName("tw_led_channel_t_r")
        self.gridLayout_15 = QtWidgets.QGridLayout(self.tw_led_channel_t_r)
        self.gridLayout_15.setObjectName("gridLayout_15")
        self.f_tw_led_channel_t_r_ph = QtWidgets.QFrame(self.tw_led_channel_t_r)
        self.f_tw_led_channel_t_r_ph.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_tw_led_channel_t_r_ph.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_tw_led_channel_t_r_ph.setObjectName("f_tw_led_channel_t_r_ph")
        self.gl_tw_led_channel_t_r_ph = QtWidgets.QGridLayout(self.f_tw_led_channel_t_r_ph)
        self.gl_tw_led_channel_t_r_ph.setContentsMargins(0, 0, 0, 0)
        self.gl_tw_led_channel_t_r_ph.setSpacing(0)
        self.gl_tw_led_channel_t_r_ph.setObjectName("gl_tw_led_channel_t_r_ph")
        self.gridLayout_15.addWidget(self.f_tw_led_channel_t_r_ph, 0, 0, 1, 1)
        self.tw_led_curve_channel.addTab(self.tw_led_channel_t_r, "")
        self.tw_led_channel_t_g = QtWidgets.QWidget()
        self.tw_led_channel_t_g.setObjectName("tw_led_channel_t_g")
        self.gridLayout_17 = QtWidgets.QGridLayout(self.tw_led_channel_t_g)
        self.gridLayout_17.setObjectName("gridLayout_17")
        self.f_tw_led_channel_t_g_ph = QtWidgets.QFrame(self.tw_led_channel_t_g)
        self.f_tw_led_channel_t_g_ph.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_tw_led_channel_t_g_ph.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_tw_led_channel_t_g_ph.setObjectName("f_tw_led_channel_t_g_ph")
        self.gl_tw_led_channel_t_g_ph = QtWidgets.QGridLayout(self.f_tw_led_channel_t_g_ph)
        self.gl_tw_led_channel_t_g_ph.setContentsMargins(0, 0, 0, 0)
        self.gl_tw_led_channel_t_g_ph.setSpacing(0)
        self.gl_tw_led_channel_t_g_ph.setObjectName("gl_tw_led_channel_t_g_ph")
        self.gridLayout_17.addWidget(self.f_tw_led_channel_t_g_ph, 0, 0, 1, 1)
        self.tw_led_curve_channel.addTab(self.tw_led_channel_t_g, "")
        self.tw_led_channel_t_b = QtWidgets.QWidget()
        self.tw_led_channel_t_b.setObjectName("tw_led_channel_t_b")
        self.gridLayout_18 = QtWidgets.QGridLayout(self.tw_led_channel_t_b)
        self.gridLayout_18.setObjectName("gridLayout_18")
        self.f_tw_led_channel_t_b_ph = QtWidgets.QFrame(self.tw_led_channel_t_b)
        self.f_tw_led_channel_t_b_ph.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_tw_led_channel_t_b_ph.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_tw_led_channel_t_b_ph.setObjectName("f_tw_led_channel_t_b_ph")
        self.gl_tw_led_channel_t_b_ph = QtWidgets.QGridLayout(self.f_tw_led_channel_t_b_ph)
        self.gl_tw_led_channel_t_b_ph.setContentsMargins(0, 0, 0, 0)
        self.gl_tw_led_channel_t_b_ph.setSpacing(0)
        self.gl_tw_led_channel_t_b_ph.setObjectName("gl_tw_led_channel_t_b_ph")
        self.gridLayout_18.addWidget(self.f_tw_led_channel_t_b_ph, 0, 0, 1, 1)
        self.tw_led_curve_channel.addTab(self.tw_led_channel_t_b, "")
        self.gl_led_curve_editor_anim.addWidget(self.tw_led_curve_channel, 1, 0, 1, 1)
        self.gb_led_curve_options = QtWidgets.QGroupBox(self.f_led_curve_editor_anim)
        self.gb_led_curve_options.setCheckable(True)
        self.gb_led_curve_options.setObjectName("gb_led_curve_options")
        self.gridLayout_16 = QtWidgets.QGridLayout(self.gb_led_curve_options)
        self.gridLayout_16.setObjectName("gridLayout_16")
        self.f_led_options_anim = QtWidgets.QFrame(self.gb_led_curve_options)
        self.f_led_options_anim.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_led_options_anim.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_led_options_anim.setObjectName("f_led_options_anim")
        self.gl_led_options_anim = QtWidgets.QGridLayout(self.f_led_options_anim)
        self.gl_led_options_anim.setContentsMargins(0, 0, 0, 0)
        self.gl_led_options_anim.setSpacing(0)
        self.gl_led_options_anim.setObjectName("gl_led_options_anim")
        self.f_led_options_preset = QtWidgets.QFrame(self.f_led_options_anim)
        self.f_led_options_preset.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_led_options_preset.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_led_options_preset.setObjectName("f_led_options_preset")
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout(self.f_led_options_preset)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.label_7 = QtWidgets.QLabel(self.f_led_options_preset)
        self.label_7.setObjectName("label_7")
        self.horizontalLayout_2.addWidget(self.label_7)
        self.cb_led_options_preset = QtWidgets.QComboBox(self.f_led_options_preset)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.cb_led_options_preset.sizePolicy().hasHeightForWidth())
        self.cb_led_options_preset.setSizePolicy(sizePolicy)
        self.cb_led_options_preset.setEditable(True)
        self.cb_led_options_preset.setInsertPolicy(QtWidgets.QComboBox.InsertAlphabetically)
        self.cb_led_options_preset.setObjectName("cb_led_options_preset")
        self.horizontalLayout_2.addWidget(self.cb_led_options_preset)
        self.pb_led_options_save = QtWidgets.QPushButton(self.f_led_options_preset)
        self.pb_led_options_save.setObjectName("pb_led_options_save")
        self.horizontalLayout_2.addWidget(self.pb_led_options_save)
        self.pb_led_options_load = QtWidgets.QPushButton(self.f_led_options_preset)
        self.pb_led_options_load.setObjectName("pb_led_options_load")
        self.horizontalLayout_2.addWidget(self.pb_led_options_load)
        self.gl_led_options_anim.addWidget(self.f_led_options_preset, 0, 0, 1, 1)
        self.f_led_curve_control_selection = QtWidgets.QFrame(self.f_led_options_anim)
        self.f_led_curve_control_selection.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_led_curve_control_selection.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_led_curve_control_selection.setObjectName("f_led_curve_control_selection")
        self.formLayout_2 = QtWidgets.QFormLayout(self.f_led_curve_control_selection)
        self.formLayout_2.setObjectName("formLayout_2")
        self.rb_time_controlled = QtWidgets.QRadioButton(self.f_led_curve_control_selection)
        self.rb_time_controlled.setChecked(True)
        self.rb_time_controlled.setObjectName("rb_time_controlled")
        self.formLayout_2.setWidget(0, QtWidgets.QFormLayout.LabelRole, self.rb_time_controlled)
        self.rb_temperature_controlled = QtWidgets.QRadioButton(self.f_led_curve_control_selection)
        self.rb_temperature_controlled.setObjectName("rb_temperature_controlled")
        self.formLayout_2.setWidget(0, QtWidgets.QFormLayout.FieldRole, self.rb_temperature_controlled)
        self.gl_led_options_anim.addWidget(self.f_led_curve_control_selection, 2, 0, 1, 1)
        self.f_led_curve_temperature = QtWidgets.QFrame(self.f_led_options_anim)
        self.f_led_curve_temperature.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_led_curve_temperature.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_led_curve_temperature.setObjectName("f_led_curve_temperature")
        self.horizontalLayout_4 = QtWidgets.QHBoxLayout(self.f_led_curve_temperature)
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.label_5 = QtWidgets.QLabel(self.f_led_curve_temperature)
        self.label_5.setObjectName("label_5")
        self.horizontalLayout_4.addWidget(self.label_5)
        self.cb_led_curve_temperature_source_selection = QtWidgets.QComboBox(self.f_led_curve_temperature)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.cb_led_curve_temperature_source_selection.sizePolicy().hasHeightForWidth())
        self.cb_led_curve_temperature_source_selection.setSizePolicy(sizePolicy)
        self.cb_led_curve_temperature_source_selection.setEditable(True)
        self.cb_led_curve_temperature_source_selection.setInsertPolicy(QtWidgets.QComboBox.NoInsert)
        self.cb_led_curve_temperature_source_selection.setObjectName("cb_led_curve_temperature_source_selection")
        self.horizontalLayout_4.addWidget(self.cb_led_curve_temperature_source_selection)
        self.gl_led_options_anim.addWidget(self.f_led_curve_temperature, 1, 0, 1, 1)
        self.f_channel_sync_options = QtWidgets.QFrame(self.f_led_options_anim)
        self.f_channel_sync_options.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_channel_sync_options.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_channel_sync_options.setObjectName("f_channel_sync_options")
        self.gridLayout_13 = QtWidgets.QGridLayout(self.f_channel_sync_options)
        self.gridLayout_13.setObjectName("gridLayout_13")
        self.label = QtWidgets.QLabel(self.f_channel_sync_options)
        self.label.setObjectName("label")
        self.gridLayout_13.addWidget(self.label, 0, 0, 1, 1)
        self.cb_r_channel_sync = QtWidgets.QComboBox(self.f_channel_sync_options)
        self.cb_r_channel_sync.setObjectName("cb_r_channel_sync")
        self.gridLayout_13.addWidget(self.cb_r_channel_sync, 1, 0, 1, 1)
        self.cb_g_channel_sync = QtWidgets.QComboBox(self.f_channel_sync_options)
        self.cb_g_channel_sync.setObjectName("cb_g_channel_sync")
        self.gridLayout_13.addWidget(self.cb_g_channel_sync, 1, 1, 1, 1)
        self.label_2 = QtWidgets.QLabel(self.f_channel_sync_options)
        self.label_2.setObjectName("label_2")
        self.gridLayout_13.addWidget(self.label_2, 0, 1, 1, 1)
        self.label_3 = QtWidgets.QLabel(self.f_channel_sync_options)
        self.label_3.setObjectName("label_3")
        self.gridLayout_13.addWidget(self.label_3, 0, 2, 1, 1)
        self.cb_b_channel_sync = QtWidgets.QComboBox(self.f_channel_sync_options)
        self.cb_b_channel_sync.setObjectName("cb_b_channel_sync")
        self.gridLayout_13.addWidget(self.cb_b_channel_sync, 1, 2, 1, 1)
        self.gl_led_options_anim.addWidget(self.f_channel_sync_options, 3, 0, 1, 1)
        self.gridLayout_16.addWidget(self.f_led_options_anim, 2, 0, 1, 1)
        self.gl_led_curve_editor_anim.addWidget(self.gb_led_curve_options, 2, 0, 1, 1)
        self.cb_led_curve_selection = QtWidgets.QComboBox(self.f_led_curve_editor_anim)
        self.cb_led_curve_selection.setEditable(True)
        self.cb_led_curve_selection.setInsertPolicy(QtWidgets.QComboBox.NoInsert)
        self.cb_led_curve_selection.setObjectName("cb_led_curve_selection")
        self.gl_led_curve_editor_anim.addWidget(self.cb_led_curve_selection, 0, 0, 1, 1)
        self.gridLayout_14.addWidget(self.f_led_curve_editor_anim, 1, 0, 1, 1)
        self.gl_sawc_chart.addWidget(self.gb_led_curve_editor, 2, 0, 1, 1)
        self.sa_chart.setWidget(self.sawc_chart)
        self.f_fan_and_preview = QtWidgets.QFrame(self.sp_main)
        self.f_fan_and_preview.setMinimumSize(QtCore.QSize(285, 0))
        self.f_fan_and_preview.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.f_fan_and_preview.setFrameShadow(QtWidgets.QFrame.Raised)
        self.f_fan_and_preview.setObjectName("f_fan_and_preview")
        self.gridLayout = QtWidgets.QGridLayout(self.f_fan_and_preview)
        self.gridLayout.setObjectName("gridLayout")
        self.gb_fan_status = QtWidgets.QGroupBox(self.f_fan_and_preview)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.gb_fan_status.sizePolicy().hasHeightForWidth())
        self.gb_fan_status.setSizePolicy(sizePolicy)
        self.gb_fan_status.setObjectName("gb_fan_status")
        self.gridLayout_10 = QtWidgets.QGridLayout(self.gb_fan_status)
        self.gridLayout_10.setObjectName("gridLayout_10")
        self.sa_fan_status = QtWidgets.QScrollArea(self.gb_fan_status)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sa_fan_status.sizePolicy().hasHeightForWidth())
        self.sa_fan_status.setSizePolicy(sizePolicy)
        self.sa_fan_status.setWidgetResizable(True)
        self.sa_fan_status.setObjectName("sa_fan_status")
        self.sawc_fan_status = QtWidgets.QWidget()
        self.sawc_fan_status.setGeometry(QtCore.QRect(0, 0, 233, 473))
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sawc_fan_status.sizePolicy().hasHeightForWidth())
        self.sawc_fan_status.setSizePolicy(sizePolicy)
        self.sawc_fan_status.setObjectName("sawc_fan_status")
        self.gridLayout_9 = QtWidgets.QGridLayout(self.sawc_fan_status)
        self.gridLayout_9.setObjectName("gridLayout_9")
        self.sa_fan_status.setWidget(self.sawc_fan_status)
        self.gridLayout_10.addWidget(self.sa_fan_status, 0, 0, 1, 1)
        self.gridLayout.addWidget(self.gb_fan_status, 0, 0, 1, 1)
        self.gb_led_preview = QtWidgets.QGroupBox(self.f_fan_and_preview)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.gb_led_preview.sizePolicy().hasHeightForWidth())
        self.gb_led_preview.setSizePolicy(sizePolicy)
        self.gb_led_preview.setObjectName("gb_led_preview")
        self.gl_led_preview = QtWidgets.QGridLayout(self.gb_led_preview)
        self.gl_led_preview.setContentsMargins(0, 6, 0, 0)
        self.gl_led_preview.setSpacing(0)
        self.gl_led_preview.setObjectName("gl_led_preview")
        self.gridLayout.addWidget(self.gb_led_preview, 1, 0, 1, 1)
        self.gridLayout_2.addWidget(self.sp_main, 0, 0, 1, 1)
        mw_main.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(mw_main)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1024, 24))
        self.menubar.setObjectName("menubar")
        self.menuFile = QtWidgets.QMenu(self.menubar)
        self.menuFile.setObjectName("menuFile")
        self.menuPreferences = QtWidgets.QMenu(self.menubar)
        self.menuPreferences.setObjectName("menuPreferences")
        mw_main.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(mw_main)
        self.statusbar.setObjectName("statusbar")
        mw_main.setStatusBar(self.statusbar)
        self.actionExit = QtWidgets.QAction(mw_main)
        self.actionExit.setObjectName("actionExit")
        self.actionSet_Refresh_Rate = QtWidgets.QAction(mw_main)
        self.actionSet_Refresh_Rate.setObjectName("actionSet_Refresh_Rate")
        self.menuFile.addAction(self.actionExit)
        self.menuPreferences.addAction(self.actionSet_Refresh_Rate)
        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuPreferences.menuAction())

        self.retranslateUi(mw_main)
        self.tw_led_curve_channel.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(mw_main)
        mw_main.setTabOrder(self.sa_chart, self.gb_temperature_display)
        mw_main.setTabOrder(self.gb_temperature_display, self.cb_temperature_display_selection)
        mw_main.setTabOrder(self.cb_temperature_display_selection, self.gb_fan_curve_editor)
        mw_main.setTabOrder(self.gb_fan_curve_editor, self.cb_fan_curve_selection)
        mw_main.setTabOrder(self.cb_fan_curve_selection, self.gb_fan_curve_options)
        mw_main.setTabOrder(self.gb_fan_curve_options, self.cb_fan_options_preset)
        mw_main.setTabOrder(self.cb_fan_options_preset, self.pb_fan_options_save)
        mw_main.setTabOrder(self.pb_fan_options_save, self.pb_fan_options_load)
        mw_main.setTabOrder(self.pb_fan_options_load, self.cb_fan_curve_temperature_source_selection)
        mw_main.setTabOrder(self.cb_fan_curve_temperature_source_selection, self.cb_pwm_controlled)
        mw_main.setTabOrder(self.cb_pwm_controlled, self.gb_led_curve_editor)
        mw_main.setTabOrder(self.gb_led_curve_editor, self.cb_led_curve_selection)
        mw_main.setTabOrder(self.cb_led_curve_selection, self.tw_led_curve_channel)
        mw_main.setTabOrder(self.tw_led_curve_channel, self.gb_led_curve_options)
        mw_main.setTabOrder(self.gb_led_curve_options, self.cb_led_options_preset)
        mw_main.setTabOrder(self.cb_led_options_preset, self.pb_led_options_save)
        mw_main.setTabOrder(self.pb_led_options_save, self.pb_led_options_load)
        mw_main.setTabOrder(self.pb_led_options_load, self.cb_led_curve_temperature_source_selection)
        mw_main.setTabOrder(self.cb_led_curve_temperature_source_selection, self.rb_time_controlled)
        mw_main.setTabOrder(self.rb_time_controlled, self.rb_temperature_controlled)
        mw_main.setTabOrder(self.rb_temperature_controlled, self.cb_r_channel_sync)
        mw_main.setTabOrder(self.cb_r_channel_sync, self.cb_g_channel_sync)
        mw_main.setTabOrder(self.cb_g_channel_sync, self.cb_b_channel_sync)
        mw_main.setTabOrder(self.cb_b_channel_sync, self.sa_fan_status)

    def retranslateUi(self, mw_main):
        _translate = QtCore.QCoreApplication.translate
        mw_main.setWindowTitle(_translate("mw_main", "Leviathan\'s Breath"))
        self.gb_fan_curve_editor.setTitle(_translate("mw_main", "Fan Curve Editor"))
        self.gb_fan_curve_options.setTitle(_translate("mw_main", "Options"))
        self.label_6.setText(_translate("mw_main", "Preset"))
        self.pb_fan_options_save.setText(_translate("mw_main", "Save"))
        self.pb_fan_options_load.setText(_translate("mw_main", "Load"))
        self.label_4.setText(_translate("mw_main", "Temperature Source:"))
        self.cb_pwm_controlled.setText(_translate("mw_main", "PWM Controlled"))
        self.gb_temperature_display.setTitle(_translate("mw_main", "Temperature"))
        self.gb_led_curve_editor.setTitle(_translate("mw_main", "LED Curve Editor"))
        self.tw_led_curve_channel.setTabText(self.tw_led_curve_channel.indexOf(self.tw_led_channel_t_r), _translate("mw_main", "R"))
        self.tw_led_curve_channel.setTabText(self.tw_led_curve_channel.indexOf(self.tw_led_channel_t_g), _translate("mw_main", "G"))
        self.tw_led_curve_channel.setTabText(self.tw_led_curve_channel.indexOf(self.tw_led_channel_t_b), _translate("mw_main", "B"))
        self.gb_led_curve_options.setTitle(_translate("mw_main", "Options"))
        self.label_7.setText(_translate("mw_main", "Preset"))
        self.pb_led_options_save.setText(_translate("mw_main", "Save"))
        self.pb_led_options_load.setText(_translate("mw_main", "Load"))
        self.rb_time_controlled.setText(_translate("mw_main", "Time Controlled"))
        self.rb_temperature_controlled.setText(_translate("mw_main", "Temperature Controlled"))
        self.label_5.setText(_translate("mw_main", "Temperature Source:"))
        self.label.setText(_translate("mw_main", "Sync R Channel To..."))
        self.label_2.setText(_translate("mw_main", "Sync G Channel To..."))
        self.label_3.setText(_translate("mw_main", "Sync B Channel To..."))
        self.gb_fan_status.setTitle(_translate("mw_main", "Fan Status"))
        self.gb_led_preview.setTitle(_translate("mw_main", "LED Preview"))
        self.menuFile.setTitle(_translate("mw_main", "File"))
        self.menuPreferences.setTitle(_translate("mw_main", "Preferences"))
        self.actionExit.setText(_translate("mw_main", "Exit"))
        self.actionSet_Refresh_Rate.setText(_translate("mw_main", "Set Refresh Rate..."))


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    mw_main = QtWidgets.QMainWindow()
    ui = Ui_mw_main()
    ui.setupUi(mw_main)
    mw_main.show()
    sys.exit(app.exec_())
