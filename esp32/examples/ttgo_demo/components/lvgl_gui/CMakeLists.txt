idf_component_register(
        SRC_DIRS
            .
            lvgl/lv_core
            lvgl/lv_draw
            lvgl/lv_fonts
            lvgl/lv_hal
            lvgl/lv_misc
            lvgl/lv_objx
            lvgl/lv_themes

        INCLUDE_DIRS
            .
            include
            lvgl
            lvgl/lv_core
            lvgl/lv_draw
            lvgl/lv_fonts
            lvgl/lv_hal
            lvgl/lv_misc
            lvgl/lv_objx
            lvgl/lv_themes

        PRIV_INCLUDE_DIRS
            .
            include
            lvgl
            lvgl/lv_core
            lvgl/lv_draw
            lvgl/lv_fonts
            lvgl/lv_hal
            lvgl/lv_misc
            lvgl/lv_objx
            lvgl/lv_themes

        PRIV_REQUIRES
            main
)