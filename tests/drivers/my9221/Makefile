include ../Makefile.drivers_common

USEMODULE += my9221

# disable native GPIOs for automatic test
ifneq (,$(filter native native32 native64,$(BOARD)))
  USEMODULE += periph_gpio_mock
endif

# set default device parameters in case they are undefined
# the following params are for board pba-d-01-kw2x and pins PA01 and PA02
TEST_MY9221_CLK ?= GPIO_PIN\(0,1\)
TEST_MY9221_DAT ?= GPIO_PIN\(0,2\)
TEST_MY9221_DIR ?= MY9221_DIR_FWD
TEST_MY9221_NUM ?= 10

# export parameters
CFLAGS += -DTEST_MY9221_CLK=$(TEST_MY9221_CLK)
CFLAGS += -DTEST_MY9221_DAT=$(TEST_MY9221_DAT)
CFLAGS += -DTEST_MY9221_DIR=$(TEST_MY9221_DIR)
CFLAGS += -DTEST_MY9221_NUM=$(TEST_MY9221_NUM)

include $(RIOTBASE)/Makefile.include
