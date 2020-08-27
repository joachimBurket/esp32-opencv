idf_component_register(
        SRC_DIRS
            .
            lvgl/src/lv_core
            lvgl/src/lv_draw
            lvgl/src/lv_font
            lvgl/src/lv_hal
            lvgl/src/lv_misc
            lvgl/src/lv_widgets
            lvgl/src/lv_themes

        INCLUDE_DIRS
            .
            include
            lvgl
            lvgl/src/lv_core
            lvgl/src/lv_draw
            lvgl/src/lv_font
            lvgl/src/lv_hal
            lvgl/src/lv_misc
            lvgl/src/lv_widgets
            lvgl/src/lv_themes

        PRIV_INCLUDE_DIRS
            .
            include
            lvgl
            lvgl/src/lv_core
            lvgl/src/lv_draw
            lvgl/src/lv_font
            lvgl/src/lv_hal
            lvgl/src/lv_misc
            lvgl/src/lv_widgets
            lvgl/src/lv_themes

        PRIV_REQUIRES
            main
)