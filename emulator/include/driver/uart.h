typedef struct {
    int baud_rate;                      /*!< UART baud rate*/
    unsigned int data_bits;       /*!< UART byte size*/
    unsigned int parity;               /*!< UART parity mode*/
    unsigned int stop_bits;         /*!< UART stop bits*/
    unsigned int flow_ctrl;    /*!< UART HW flow control mode (cts/rts)*/
    unsigned int rx_flow_ctrl_thresh;        /*!< UART HW RTS threshold*/
    unsigned int source_clk;         /*!< UART source clock selection */
} uart_config_t;
