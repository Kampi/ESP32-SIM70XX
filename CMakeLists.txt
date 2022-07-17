set(COMPONENT_SRCS
    # Common
    "src/sim70xx_tools.cpp"

    # SIM7080
    "src/SIM7080/sim7080.cpp"
    "src/SIM7080/Misc/sim7080_info.cpp"

    # SIM7020
    "src/SIM7020/sim7020.cpp"
    "src/SIM7020/Protocols/sim7020_coap.cpp"
    "src/SIM7020/Protocols/sim7020_dns.cpp"
    "src/SIM7020/Protocols/sim7020_http.cpp"
    "src/SIM7020/Protocols/sim7020_mqtt.cpp"
    "src/SIM7020/Protocols/sim7020_ping.cpp"
    "src/SIM7020/Protocols/sim7020_sntp.cpp"
    "src/SIM7020/Protocols/sim7020_tcp.cpp"
    "src/SIM7020/PowerManagement/sim7020_pwrmgnt.cpp"
    "src/SIM7020/Misc/sim7020_info.cpp"
    "src/SIM7020/Misc/sim7020_nvram.cpp"
    "src/SIM7020/Misc/sim7020_pdp.cpp"
    "src/Private/Events/SIM7020/sim7020_evt_http.cpp"
    "src/Private/Events/SIM7020/sim7020_evt_mqtt.cpp"
    "src/Private/Events/SIM7020/sim7020_evt_tcp.cpp"
    "src/Private/Events/SIM7020/sim7020_evt_psm.cpp"

    # Driver internal
    "src/Private/Events/sim70xx_evt.cpp"
    "src/Private/Queue/sim70xx_queue.cpp"
    "src/Private/UART/sim70xx_uart.cpp"
    )

set(COMPONENT_ADD_INCLUDEDIRS
	"include"

    # SIM7080
    "include/SIM7080"

    # SIM7020
    "include/SIM7020"
	"include/SIM7020/Misc"
	"include/SIM7020/Protocols"
	"include/SIM7020/Definitions"
	"include/SIM7020/PowerManagement"
	"include/SIM7020/Definitions/Misc"
	"include/SIM7020/Definitions/Configs"
	"include/SIM7020/Definitions/Protocols"
	"include/SIM7020/Definitions/PowerManagement"
	)

set(COMPONENT_PRIV_REQUIRES freertos mbedtls)

register_component()