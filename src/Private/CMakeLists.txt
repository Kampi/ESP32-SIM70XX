set(COMPONENT_SRCS
    "src/Private/Events/sim70xx_evt.cpp"
    "src/Private/Queue/sim70xx_queue.cpp"
    "src/Private/UART/sim70xx_uart.cpp"
    )

set(COMPONENT_ADD_INCLUDEDIRS
	)

set(COMPONENT_PRIV_REQUIRES freertos mbedtls)

register_component()