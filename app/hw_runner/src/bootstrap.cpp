// PC13 - LED
// PA9 - USART1 TX
// PB12 - Button (short to GND)

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include "HW.hpp"
#include "libopencm3/stm32/exti.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/usart.h"
#include "libopencmsis/core_cm3.h"

class GpioButton : public IButton
{
public:
    void InterruptHandler()
    {
        exti_reset_request(EXTI12);
        _pressed = true;
    }
    void WaitForPress() override
    {
        exti_select_source(EXTI12, GPIOC);
        exti_set_trigger(EXTI12, EXTI_TRIGGER_FALLING);
        exti_reset_request(EXTI12);
        _pressed = false;
        exti_enable_request(EXTI12);
        while(!_pressed)
        {
            asm volatile("wfi");
        }
        exti_disable_request(EXTI12);
    }

private:
    volatile bool _pressed{false};
};

class UsartLog : public ILog
{
public:
    explicit UsartLog(std::uint32_t hw);

    void Log(const char* fmt, ...) override;

private:
    std::uint32_t _hw;
};

UsartLog::UsartLog(std::uint32_t hw) : _hw{hw}
{
}

void UsartLog::Log(const char* fmt, ...)
{
    char buf[200] = {0};

    va_list args;
    va_start(args, fmt);

    auto n = vsniprintf(buf, sizeof(buf), fmt, args);

    for(auto p = buf; p != buf + n; p++)
    {
        usart_send_blocking(_hw, *p);
    }
    usart_send_blocking(_hw, '\n');

    va_end(args);
}

class STMHardware : public IHardware
{
public:
    void Initialize();
    void ButtonInterruptHandler();
    ILog& Log() override;
    IButton& Button() override;

private:
    UsartLog _log{USART3};
    GpioButton _button;
};

static void clock_setup(void) {
    // Enable external high-speed oscillator (HSE)
    rcc_osc_on(RCC_HSE);
    rcc_wait_for_osc_ready(RCC_HSE);

    // Configure the PLL
    // HSE = 8 MHz (typical for Nucleo-144 boards)
    // Target System Clock = 120 MHz
    // PLLM = 8, PLLN = 240, PLLP = 2, PLLQ = 5
    rcc_set_main_pll_hse(8, 240, 2, 5);

    // Enable the PLL
    rcc_osc_on(RCC_PLL);
    rcc_wait_for_osc_ready(RCC_PLL);

    // Select PLL as the system clock source
    rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
    rcc_wait_for_sysclk_status(RCC_PLL);

    // Set prescalers for AHB, APB1, and APB2
    rcc_set_hpre(RCC_CFGR_HPRE_DIV_NONE);  // AHB = SYSCLK / 1
    rcc_set_ppre1(RCC_CFGR_PPRE_DIV_4);    // APB1 = SYSCLK / 4 (Max 30 MHz)
    rcc_set_ppre2(RCC_CFGR_PPRE_DIV_2);    // APB2 = SYSCLK / 2 (Max 60 MHz)

    // Update the SystemCoreClock variable
    rcc_ahb_frequency = 120000000;
    rcc_apb1_frequency = 120000000 / 4;
    rcc_apb2_frequency = 120000000 / 2;
}


static void gpio_setup(void)
{
    // Enable GPIOB clock
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    // configure Pin 0 Port B
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO0);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO0);
}

void STMHardware::Initialize()
{
    clock_setup();
    gpio_setup();

    rcc_periph_clock_enable(RCC_USART3);

    usart_set_baudrate(USART3, 115200);
    usart_set_databits(USART3, 8);
    usart_set_stopbits(USART3, USART_STOPBITS_1);
    usart_set_mode(USART3, USART_MODE_TX);
    usart_set_parity(USART3, USART_PARITY_NONE);
    usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);
    usart_enable(USART3);
}

void STMHardware::ButtonInterruptHandler()
{
    _button.InterruptHandler();
}

ILog& STMHardware::Log()
{
    return _log;
}

IButton& STMHardware::Button()
{
    return _button;
}

static STMHardware& GetSTMHardware()
{
    static STMHardware hw;
    return hw;
}

IHardware& GetHardware()
{
    return GetSTMHardware();
}

extern "C" void exti15_10_isr()
{
    GetSTMHardware().ButtonInterruptHandler();
}

extern void run();

int main()
{
    GetSTMHardware().Initialize();

    run();

    //while(true)
    //{
    //    gpio_clear(GPIOB, GPIO7);
    //    for(int i = 0; i < 400'000; i++)
    //    {
    //        asm volatile("nop");
    //    }
    //    gpio_set(GPIOB, GPIO7);
    //    for(int i = 0; i < 400'000; i++)
    //    {
    //        asm volatile("nop");
    //    }
    //     usart_send_blocking(USART1, 'A');
    //}
    return 0;
}
