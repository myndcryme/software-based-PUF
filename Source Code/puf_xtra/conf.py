# Configuration file for software-based-PUF,
# https://github.com/Tribler/software-based-PUF
# Copyright (C) 2023 myndcryme
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from pconst import const
from pconst import const as conf    # dual alias for dual prefix naming (caveat: conf and const are interchangeable)

#################################################
# configuration values are immutable at runtime #
#################################################

""" OPERATION CONFIG """
conf.OPERATION_MODE = 'MONO'            # single arduino
conf.SERIAL_DEVICE = '2341:0042'        # either device VID:PID or serial device path works

#conf.OPERATION_MODE = 'PARALLEL'            # for parallel profiling
#conf.SERIAL_DEVICE = ['/dev/ttyACM1']       # test case
#conf.SERIAL_DEVICE = ['/dev/ttyACM0', '/dev/ttyACM1', '/dev/ttyACM2']   # use device path only, add as needed

""" SRAM MODULE CONFIG """
conf.SRAM = 'CY62256NLL'
#conf.SRAM = '23LC1024'

#GSBV_MODE not yet implemented
#conf.GSBV_MODE = 'VOLTAGE_VARIATION'       # select a mode when running GetStableBitsValue.py (GSBV)
#conf.GSBV_MODE = 'TIME_INTERVAL'

""" GENERAL CONFIG """
conf.BITRATE = 115200       # serial connection bitrate setting
conf.USE_BINDING = True
conf.URANDOM_SOURCE = 'OS_URANDOM'          # 'DEV_URANDOM'

conf.DEBUG = False          # project debug
conf.DEBUG_FILE = False     # file debug

############################################
# do not change config param entries below #
############################################

conf.ERR_THRESH = 0.10      # max percent error (dissimilar cells) allowed for positive SRAM identification
conf.BIND_FILE = 'bindings.csv'
conf.USER_FILE = 'user.py'
conf.USE_UPDATE = conf.USE_BINDING
conf.USE_LEGACY = not conf.USE_BINDING

####################################################
# immutable constants - do not alter entries below #
####################################################

const.DEFAULT_DEVICE_ID = '2341:0042'                   # Genuine Arduino MEGA2560_R3 VID:PID
const.DEFAULT_DEVICE_NAME = 'GENUINE_MEGA2560_R3'
const.GET_UID = 0x37            # 55
const.UID_BUF_SIZE = 36
const.SEED_ARDUINO_CMD = 0x39   # 57
const.SEED_BUF_SIZE = 5         # one byte prefix, 4 bytes to construct seed value
const.EMPTY = ''
const.RUN = 'RUN'
const.RERUN = const.RUN
const.FALSE = 0
const.TRUE = 1
