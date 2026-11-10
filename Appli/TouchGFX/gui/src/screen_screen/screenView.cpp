#include <gui/screen_screen/screenView.hpp>
#include <images/BitmapDatabase.hpp>
#include "main.h"

screenView::screenView() :
    ethTestButtonCallback(this, &screenView::ethTestButtonClicked),
    displayedEthTestResult(ETH_TEST_NOT_READY),
    bitmapSwitchTick(0U),
    bitmapTestPressed(false)
{
    buttonWithLabel_ETHTEST.setClickAction(ethTestButtonCallback);
}

void screenView::setupScreen()
{
    screenViewBase::setupScreen();
    bitmapSwitchTick = HAL_GetTick();
    bitmapTestPressed = false;
    buttonWithLabel_ETHTEST_1.setBitmaps(
        touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_MEDIUM_ROUNDED_NORMAL_ID),
        touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_MEDIUM_ROUNDED_NORMAL_ID));
    buttonWithLabel_ETHTEST_1.invalidate();
}

void screenView::tearDownScreen()
{
    screenViewBase::tearDownScreen();
}

void screenView::handleTickEvent()
{
    const uint32_t currentTick = HAL_GetTick();
    if ((currentTick - bitmapSwitchTick) >= 2000U)
    {
        bitmapSwitchTick = currentTick;
        bitmapTestPressed = !bitmapTestPressed;

        const uint16_t bitmapId = bitmapTestPressed
            ? BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_MEDIUM_ROUNDED_PRESSED_ID
            : BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_MEDIUM_ROUNDED_NORMAL_ID;
        const touchgfx::Bitmap bitmap(bitmapId);
        buttonWithLabel_ETHTEST_1.setBitmaps(bitmap, bitmap);
        buttonWithLabel_ETHTEST_1.invalidate();
    }

    const ETH_TestResultTypeDef result = ETH_Loopback_GetResult();

    if (result != displayedEthTestResult)
    {
        updateEthTestResult(result);
    }
}

void screenView::ethTestButtonClicked(const touchgfx::ButtonWithLabel& source,
                                      const touchgfx::ClickEvent& event)
{
    (void)source;

    if (event.getType() == touchgfx::ClickEvent::RELEASED)
    {
        if (ETH_Loopback_RequestStart() != HAL_OK)
        {
            updateEthTestResult(ETH_TEST_FAULT);
        }
    }
}

void screenView::updateEthTestResult(ETH_TestResultTypeDef result)
{
    const char* text;

    switch (result)
    {
    case ETH_TEST_IDLE:
        text = "READY";
        break;
    case ETH_TEST_RUNNING:
        text = "RUNNING";
        break;
    case ETH_TEST_PASS:
        text = "PASS";
        break;
    case ETH_TEST_FAULT:
        text = "FAULT";
        break;
    default:
        text = "NOT READY";
        break;
    }

    touchgfx::Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(text),
                               textArea2Buffer,
                               TEXTAREA2_SIZE);
    displayedEthTestResult = result;
    textArea2.invalidate();
}
