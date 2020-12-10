idf_component_register(SRCS 
							"app_main.cpp" 
							"OLED.cpp" 
							"SSD1306.cpp" 
                    INCLUDE_DIRS 
                    		"include"
                    )
                    
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-int-in-bool-context)
