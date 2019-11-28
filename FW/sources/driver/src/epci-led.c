/**
*	epci led handler
*	Copyright (C) 2019	arash.golgol@gmail.com
*
*/

#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/leds.h>
#include "epci.h"


/**
*	epci led information
*/
struct led_info epci_led_info[] = {
	{
		.name = "d1:blue",
		.default_trigger = "none",
	},
	{
		.name = "d2:green",
		.default_trigger = "heartbeat",
	},
	{
		.name = "d3:red",
		.default_trigger = "default-on",
	}

};

struct led_platform_data epci_led_platform_data = {
	.num_leds = ARRAY_SIZE(epci_led_info),
	.leds = epci_led_info,
};

/**
*	set PWM
*/
static void epci_set_pwm(struct epci_led *led)
{
	struct pci_dev *dev = led->board->pdev;
	unsigned long  offset  = led->led_num >> 2;	/* each led has 4 bytes register */
	void __iomem *led_addr = led->board->base + 		
				 led->board->info->led_offset +
				 offset;
	/* iowrite32(0xFFFF0000, led_addr); */
	dev_info(&dev->dev, "led_addr = %p\n", led_addr);
}

/**
*
*/
static void epci_led_brightness_set(struct led_classdev *led_cdev, 
	enum led_brightness brightness)
{
	struct epci_led *led = NULL;
	
	led = container_of(led_cdev, struct epci_led, led_cdev);

	switch(brightness) {
	case LED_FULL:
		iowrite8(0x01, led->board->base + 0x8014);
		break;
	case LED_OFF:
		iowrite8(0x00, led->board->base + 0x8014);
		break;
	default:
		iowrite8(0x00, led->board->base + 0x8014);
		break;
	}
}


int  epci_leds_register(struct epci_priv * board)
{
	struct  epci_led *leds = NULL;
	struct  pci_dev  *dev = board->pdev;
	int		ret = 0;

	leds = devm_kzalloc(&dev->dev, sizeof(*leds),GFP_KERNEL);
	if(!leds) 
		return -ENOMEM;
	
	board->leds = leds;
	snprintf(leds->name, sizeof(leds->name),"epci-led:green");
	leds->led_cdev.name = leds->name;
	leds->led_cdev.brightness_set = epci_led_brightness_set;
	leds->board = board;
	leds->led_num = 0;

	// iowrite32(0xFFFF0000, board->base + 0x8014);	/* set PWM to max */

	epci_set_pwm(leds);

	ret = led_classdev_register(&dev->dev, &leds->led_cdev);
	if(ret < 0) {
		dev_err(&dev->dev, 
		"led class device registeration failed\n");
		return ret;
	} 

	return 0;	
}

void epci_leds_unregister(struct epci_priv * board)
{
	led_classdev_unregister(&board->leds->led_cdev);
}
