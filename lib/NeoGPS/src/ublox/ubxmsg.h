#ifndef UBXMSG_H
#define UBXMSG_H

//  Copyright (C) 2014-2017, SlashDevin
//
//  This file is part of NeoGPS
//
//  NeoGPS is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  NeoGPS is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.

#include "NMEAGPS_cfg.h"

// Disable the entire file if derived types are not allowed.
#ifdef NMEAGPS_DERIVED_TYPES

#include "NMEAGPS.h"
class ubloxGPS;

namespace ublox {

    enum msg_class_t
      { UBX_NAV  = 0x01,  // Navigation results
        UBX_RXM  = 0x02,  // Receiver Manager messages
        UBX_INF  = 0x04,  // Informational messages
        UBX_ACK  = 0x05,  // ACK/NAK replies to CFG messages
        UBX_CFG  = 0x06,  // Configuration input messages
        UBX_MON  = 0x0A,  // Monitoring messages
        UBX_AID  = 0x0B,  // Assist Now aiding messages
        UBX_TIM  = 0x0D,  // Timing messages
        UBX_HNR  = 0x28,  // High rate navigation results
        UBX_NMEA = 0xF0,  // NMEA Standard messages
        UBX_PUBX = 0xF1,  // NMEA proprietary messages (PUBX)
        UBX_UNK  = 0xFF
      }  __attribute__((packed));

    enum msg_id_t
      {
        UBX_ACK_NAK      = 0x00, // Reply to CFG messages
        UBX_ACK_ACK      = 0x01, // Reply to CFG messages
        UBX_CFG_MSG      = 0x01, // Configure which messages to send
        UBX_CFG_RST      = 0x04, // Reset command
        UBX_CFG_RATE     = 0x08, // Configure message rate
        UBX_CFG_NMEA     = 0x17, // Configure NMEA protocol
        UBX_CFG_NAV5     = 0x24, // Configure navigation engine settings
        UBX_MON_VER      = 0x04, // Monitor Receiver/Software version
        UBX_NAV_POSLLH   = 0x02, // Current Position
        UBX_NAV_STATUS   = 0x03, // Receiver Navigation Status
        UBX_NAV_DOP      = 0x04, // Dilutions of Precision
        UBX_NAV_ODO      = 0x09, // Odometer Solution (NEO-M8 only)
        UBX_NAV_PVT      = 0x07, // Position, Velocity and Time
        UBX_NAV_RESETODO = 0x10, // Reset Odometer (NEO-M8 only)
        UBX_NAV_VELNED   = 0x12, // Current Velocity
        UBX_NAV_TIMEGPS  = 0x20, // Current GPS Time
        UBX_NAV_TIMEUTC  = 0x21, // Current UTC Time
        UBX_NAV_SVINFO   = 0x30, // Space Vehicle Information
        UBX_HNR_PVT      = 0x00, // High rate Position, Velocity and Time
        UBX_ID_UNK   = 0xFF
      }  __attribute__((packed));

      struct msg_hdr_t {
          msg_class_t msg_class;
          msg_id_t    msg_id;
          bool same_kind( const msg_hdr_t & msg ) const
            { return (msg_class == msg.msg_class) && (msg_id == msg.msg_id); }
      }  __attribute__((packed));

      struct msg_t : msg_hdr_t {
          uint16_t length;  // should be sizeof(this)-sizeof(msg_hdr_t)
          #define UBX_MSG_LEN(msg) (sizeof(msg) - sizeof(ublox::msg_t))

          msg_t()
            {
              length    = 0;
            };
          msg_t( enum msg_class_t m, enum msg_id_t i, uint16_t l = 0 )
            {
                msg_class = m;
                msg_id    = i;
                length    = l;
            }
          void init()
            {
              uint8_t *mem = (uint8_t *) this;
              memset( &mem[ sizeof(msg_t) ], 0, length );
            }
      } __attribute__((packed));

    /**
      * Configure message intervals.
      */

    enum ubx_nmea_msg_t { // msg_id's for UBX_NMEA msg_class
        UBX_GPGGA = 0x00,
        UBX_GPGLL = 0x01,
        UBX_GPGSA = 0x02,
        UBX_GPGSV = 0x03,
        UBX_GPRMC = 0x04,
        UBX_GPVTG = 0x05,
        UBX_GPGST = 0x07,
        UBX_GPZDA = 0x08
    } __attribute__((packed));

    struct cfg_msg_t : msg_t {
        msg_class_t  cfg_msg_class;
        msg_id_t     cfg_msg;
        uint8_t      rate;

        cfg_msg_t( msg_class_t m, msg_id_t i, uint8_t r )
          : msg_t( UBX_CFG, UBX_CFG_MSG, UBX_MSG_LEN(*this) )
        {
          cfg_msg_class = m;
          cfg_msg       = i;
          rate          = r;
        };
    } __attribute__((packed));

    extern bool configNMEA( ubloxGPS &gps, NMEAGPS::nmea_msg_t msgType, uint8_t rate );
    
    // Reset command
    struct cfg_reset_t : msg_t {

      struct bbr_section_t
        {
          bool ephermeris            :1;
          bool almanac               :1;
          bool health                :1;
          bool klobuchard            :1;
          bool position              :1;
          bool clock_drift           :1;
          bool osc_param             :1;
          bool utc_param             :1;
          bool rtc                   :1;
          bool reserved1             :2;
          bool sfdr_param            :1;
          bool sfdr_veh_mon_param    :1;
          bool tct_param             :1;
          bool reserved2             :1;
          bool autonomous_orbit_param:1;
        } __attribute__((packed));

      enum reset_mode_t
        {
          HW_RESET                     = 0x00,
          CONTROLLED_SW_RESET          = 0x01,
          CONTROLLED_SW_RESET_GPS_ONLY = 0x02,
          HW_RESET_AFTER_SHUTDOWN      = 0x04,
          CONTROLLED_GPS_STOP          = 0x08,
          CONTROLLED_GPS_START         = 0x09
        } __attribute__((packed));

      bbr_section_t clear_bbr_section;
      reset_mode_t  reset_mode : 8;
      uint8_t       reserved   : 8;

      cfg_reset_t()
        : msg_t( UBX_CFG, UBX_CFG_RST, UBX_MSG_LEN(*this) )
          { init(); }

    }  __attribute__((packed));

    // Configure navigation rate
    enum time_ref_t {
      UBX_TIME_REF_UTC=0,
      UBX_TIME_REF_GPS=1
    } __attribute__((packed));

    struct cfg_rate_t : msg_t {
        uint16_t        GPS_meas_rate;
        uint16_t        nav_rate;
        enum time_ref_t time_ref:16;

        cfg_rate_t( uint16_t gr, uint16_t nr, enum time_ref_t tr )
          : msg_t( UBX_CFG, UBX_CFG_RATE, UBX_MSG_LEN(*this) )
        {
          GPS_meas_rate = gr;
          nav_rate      = nr;
          time_ref      = tr;
        }
    }  __attribute__((packed));

    //  Navigation Engine Expert Settings
    enum dyn_model_t {
        UBX_DYN_MODEL_PORTABLE   = 0,
        UBX_DYN_MODEL_STATIONARY = 2,
        UBX_DYN_MODEL_PEDESTRIAN = 3,
        UBX_DYN_MODEL_AUTOMOTIVE = 4,
        UBX_DYN_MODEL_SEA        = 5,
        UBX_DYN_MODEL_AIR_1G     = 6,
        UBX_DYN_MODEL_AIR_2G     = 7,
        UBX_DYN_MODEL_AIR_4G     = 8
    } __attribute__((packed));

    enum position_fix_t {
        UBX_POS_FIX_2D_ONLY = 1,
        UBX_POS_FIX_3D_ONLY = 2,
        UBX_POS_FIX_AUTO    = 3
    } __attribute__((packed));

    struct cfg_nav5_t : msg_t {
        struct parameter_mask_t {
            bool dyn_model            :1;
            bool min_elev             :1;
            bool fix                  :1;
            bool dr_limit             :1;
            bool pos_mask             :1;
            bool time_mask            :1;
            bool static_hold_thr      :1;
            bool dgps_timeout         :1;
            int  _unused_             :8;
        } __attribute__((packed));

        union {
          struct parameter_mask_t apply;
          uint16_t                apply_word;
        } __attribute__((packed));
                
        enum dyn_model_t       dyn_model:8;
        enum position_fix_t    fix_mode:8;
        int32_t                fixed_alt;          // m MSL x0.01
        uint32_t               fixed_alt_variance; // m^2 x0.0001
        int8_t                 min_elev;           // deg
        uint8_t                dr_limit;           // s
        uint16_t               pos_dop_mask;       // x0.1
        uint16_t               time_dop_mask;      // x0.1
        uint16_t               pos_acc_mask;       // m
        uint16_t               time_acc_mask;      // m
        uint8_t                static_hold_thr;    // cm/s
        uint8_t                dgps_timeout;       // s
        uint32_t always_zero_1;
        uint32_t always_zero_2;
        uint32_t always_zero_3;

        cfg_nav5_t() : msg_t( UBX_CFG, UBX_CFG_NAV5, UBX_MSG_LEN(*this) )
          {
            apply_word = 0xFF00;
            always_zero_1 =
            always_zero_2 =
            always_zero_3 = 0;
          }

    }  __attribute__((packed));

    // Geodetic Position Solution
    struct nav_posllh_t : msg_t {
        uint32_t time_of_week; // mS
        int32_t  lon; // deg * 1e7
        int32_t  lat; // deg * 1e7
        int32_t  height_above_ellipsoid; // mm
        int32_t  height_MSL; // mm
        uint32_t horiz_acc; // mm
        uint32_t vert_acc; // mm

        nav_posllh_t() : msg_t( UBX_NAV, UBX_NAV_POSLLH, UBX_MSG_LEN(*this) ) {};
    } __attribute__((packed));

    // Receiver Navigation Status
    struct nav_status_t : msg_t {
        uint32_t time_of_week; // mS
        enum status_t {
          NAV_STAT_NONE,
          NAV_STAT_DR_ONLY,
          NAV_STAT_2D,
          NAV_STAT_3D,
          NAV_STAT_GPS_DR,
          NAV_STAT_TIME_ONLY
        } __attribute__((packed))
            status;

        struct flags_t {
          bool gps_fix:1;
          bool diff_soln:1;
          bool week:1;
          bool time_of_week:1;
        } __attribute__((packed))
          flags;
        
        static gps_fix::status_t to_status( enum gps_fix::status_t status, flags_t flags )
        {
          if (!flags.gps_fix)
            return gps_fix::STATUS_NONE;
          if (flags.diff_soln)
            return gps_fix::STATUS_DGPS;
          return status;
        }
        
        struct {
          bool dgps_input:1;
          bool _skip_:6;
          bool map_matching:1;
        }  __attribute__((packed))
          fix_status;

        enum {
          PSM_ACQUISITION,
          PSM_TRACKING,
          PSM_POWER_OPTIMIZED_TRACKING,
          PSM_INACTIVE
        }
          power_safe:2; // FW > v7.01

        uint32_t time_to_first_fix; // ms time tag
        uint32_t uptime; // ms since startup/reset

        nav_status_t() : msg_t( UBX_NAV, UBX_NAV_STATUS, UBX_MSG_LEN(*this) ) {};
    }  __attribute__((packed));

    // Dilutions of Precision
    struct nav_dop_t : msg_t {
        uint32_t time_of_week; // mS

        uint16_t gdop; // Geometric
        uint16_t pdop; // Position
        uint16_t tdop; // Time
        uint16_t vdop; // Vertical
        uint16_t hdop; // Horizontal
        uint16_t ndop; // Northing
        uint16_t edop; // Easting

        nav_dop_t() : msg_t( UBX_NAV, UBX_NAV_DOP, UBX_MSG_LEN(*this) ) {};
    }  __attribute__((packed));

    // Odometer Solution (NEO-M8 only)
    struct nav_odo_t : msg_t {
        uint8_t   version;
        uint8_t   reserved[3];
        uint32_t  time_of_week;   // mS
        uint32_t  distance;       // m
        uint32_t  total_distance; // m
        uint32_t  distanceSTD;    // m (1-sigma)

        nav_odo_t() : msg_t( UBX_NAV, UBX_NAV_ODO, UBX_MSG_LEN(*this) ) {};
    }  __attribute__((packed));

    // Reset Odometer (NEO-M8 only)
    struct nav_resetodo_t : msg_t {
        // no payload, it's just a command

        nav_resetodo_t() : msg_t( UBX_NAV, UBX_NAV_RESETODO, UBX_MSG_LEN(*this) ) {};
    }  __attribute__((packed));

    // Position, Velocity and Time (NEO-7 or later)
    struct nav_pvt_t : msg_t {
        uint32_t time_of_week; // mS
        uint16_t year;           // 1999..2099
        uint8_t  month;          // 1..12
        uint8_t  day;            // 1..31
        uint8_t  hour;           // 0..23
        uint8_t  minute;         // 0..59
        uint8_t  second;         // 0..59
        struct valid_t {
          bool date          :1;
          bool time          :1;
          bool fully_resolved:1;
        } __attribute__((packed))
          valid;
        uint32_t time_acc;               // ns
        int32_t  second_frac;            // ns
        enum fix_t {
          UBX_NAV_PVT_FIX_NONE           = 0,
          UBX_NAV_PVT_FIX_DEAD_RECKONING = 1,
          UBX_NAV_PVT_FIX_2D             = 2,
          UBX_NAV_PVT_FIX_3D             = 3,
          UBX_NAV_PVT_FIX_GNSS_PLUS_DR   = 4,
          UBX_NAV_PVT_FIX_TIME           = 5
        } __attribute__((packed))
          status;
        struct flags_t {
          bool    validFix      :1;
          bool    dgps          :1;
          uint8_t psmState      :3;
        } __attribute__((packed))
          flags;
        uint8_t  reserved1;
        uint8_t  satellites;
        int32_t  lon;                    // deg * 1e7
        int32_t  lat;                    // deg * 1e7
        int32_t  height_above_ellipsoid; // mm
        int32_t  height_MSL;             // mm
        uint32_t horiz_acc;              // mm
        uint32_t vert_acc;               // mm
        int32_t  vel_north;              // cm/s
        int32_t  vel_east;               // cm/s
        int32_t  vel_down;               // cm/s
        uint32_t speed_2D;               // cm/s
        int32_t  heading;                // deg * 1e5
        uint32_t speed_acc;              // mm/s
        uint32_t heading_acc;            // deg * 1e5
        uint16_t pdop;                   // DOP * 100
        uint16_t reserved2;
        uint32_t reserved3;

        nav_pvt_t() : msg_t( UBX_NAV, UBX_NAV_PVT, UBX_MSG_LEN(*this) ) {};

        static gps_fix::status_t to_status( enum gps_fix::status_t status, fix_t pvt_status )
        {
          switch (pvt_status) {
            case UBX_NAV_PVT_FIX_NONE          :
              status = gps_fix::STATUS_NONE;
              break;
            case UBX_NAV_PVT_FIX_DEAD_RECKONING:
              status = gps_fix::STATUS_EST;
              break;
            case UBX_NAV_PVT_FIX_2D            :
              status = gps_fix::STATUS_STD;
              break;
            case UBX_NAV_PVT_FIX_3D            :
              status = gps_fix::STATUS_STD;
              break;
            case UBX_NAV_PVT_FIX_GNSS_PLUS_DR  :
              status = gps_fix::STATUS_STD;
              break;
            case UBX_NAV_PVT_FIX_TIME          :
              status = gps_fix::STATUS_TIME_ONLY;
              break;
          }
          return status;
        }

    }  __attribute__((packed));

    // Velocity Solution in North/East/Down
    struct nav_velned_t : msg_t {
        uint32_t time_of_week; // mS
        int32_t  vel_north;    // cm/s
        int32_t  vel_east;     // cm/s
        int32_t  vel_down;     // cm/s
        uint32_t speed_3D;     // cm/s
        uint32_t speed_2D;     // cm/s
        int32_t  heading;      // degrees * 1e5
        uint32_t speed_acc;    // cm/s
        uint32_t heading_acc;  // degrees * 1e5
        
        nav_velned_t() : msg_t( UBX_NAV, UBX_NAV_VELNED, UBX_MSG_LEN(*this) ) {};
    }  __attribute__((packed));

    // GPS Time Solution
    struct nav_timegps_t : msg_t {
        uint32_t time_of_week;   // mS
        int32_t  fractional_ToW; // nS
        int16_t  week;
        int8_t   leap_seconds;   // GPS-UTC
        struct valid_t {
          bool time_of_week:1;
          bool week:1;
          bool leap_seconds:1;
        } __attribute__((packed))
          valid;

        nav_timegps_t() : msg_t( UBX_NAV, UBX_NAV_TIMEGPS, UBX_MSG_LEN(*this) ) {};
    }  __attribute__((packed));

    // UTC Time Solution
    struct nav_timeutc_t : msg_t {
        uint32_t time_of_week;   // mS
        uint32_t time_accuracy;  // nS
        int32_t  fractional_ToW; // nS
        uint16_t year;           // 1999..2099
        uint8_t  month;          // 1..12
        uint8_t  day;            // 1..31
        uint8_t  hour;           // 0..23
        uint8_t  minute;         // 0..59
        uint8_t  second;         // 0..59
        struct valid_t {
          bool time_of_week:1;
          bool week_number:1;
          bool UTC:1;
        } __attribute__((packed))
          valid;

        nav_timeutc_t() : msg_t( UBX_NAV, UBX_NAV_TIMEUTC, UBX_MSG_LEN(*this) ) {};
    }  __attribute__((packed));

    // Space Vehicle Information
    struct nav_svinfo_t : msg_t {
        uint32_t time_of_week;   // mS
        uint8_t  num_channels;
        enum { ANTARIS_OR_4, UBLOX_5, UBLOX_6 } chipgen:8;
        uint16_t reserved2;
        struct sv_t {
          uint8_t channel; // 255 = no channel
          uint8_t id;      // Satellite ID
          bool    used_for_nav:1;
          bool    diff_corr   :1; // differential correction available
          bool    orbit_avail :1; // orbit info available
          bool    orbit_eph   :1; // orbit info is ephemeris
          bool    unhealthy   :1; // SV should not be used
          bool    orbit_alm   :1; // orbit info is Almanac Plus
          bool    orbit_AOP   :1; // orbit info is AssistNow Autonomous
          bool    smoothed    :1; // Carrier smoothed pseudorange used
          enum {
              IDLE, 
              SEARCHING, 
              ACQUIRED, 
              UNUSABLE, 
              CODE_LOCK, 
              CODE_AND_CARRIER_LOCK_1,
              CODE_AND_CARRIER_LOCK_2,
              CODE_AND_CARRIER_LOCK_3
            }
              quality:8;
          uint8_t  snr;           // dbHz
          uint8_t  elevation;     // degrees
          uint16_t azimuth;       // degrees
          uint32_t pr_res;        // pseudo range residual in cm
        };

        //  Calculate the number of bytes required to hold the
        //  specified number of channels.
        static uint16_t size_for( uint8_t channels )
          { return sizeof(nav_svinfo_t) + (uint16_t)channels * sizeof(sv_t); }

        // Initialze the msg_hdr for the specified number of channels
        void init( uint8_t max_channels )
        {
          msg_class = UBX_NAV;
          msg_id    = UBX_NAV_SVINFO;
          length    = size_for( max_channels ) - sizeof(ublox::msg_t);
        }

      }  __attribute__((packed));

    // High Rate PVT
    struct hnr_pvt_t : msg_t {
        uint32_t time_of_week;   // mS
        uint16_t year;           // 1999..2099
        uint8_t  month;          // 1..12
        uint8_t  day;            // 1..31
        uint8_t  hour;           // 0..23
        uint8_t  minute;         // 0..59
        uint8_t  second;         // 0..59
        struct valid_t {
            bool date:1;
            bool time:1;
            bool fully_resolved:1;
        } __attribute__((packed))
        valid;
        int32_t  fractional_ToW; // nS

        nav_status_t::status_t status;

        struct flags_t {
            bool gps_fix:1;
            bool diff_soln:1;
            bool week:1;
            bool time_of_week:1;
            bool heading_valid:1;
        } __attribute__((packed))
        flags;

        static gps_fix::status_t to_status( enum gps_fix::status_t status, flags_t flags )
        {
            if (!flags.gps_fix)
              return gps_fix::STATUS_NONE;
            if (flags.diff_soln)
              return gps_fix::STATUS_DGPS;
            return status;
        }

        uint16_t reserved1;

        int32_t  lon; // deg * 1e7
        int32_t  lat; // deg * 1e7
        int32_t  height_above_ellipsoid; // mm
        int32_t  height_MSL; // mm

        int32_t  speed_2D;        // mm/s
        int32_t  speed_3D;        // mm/s
        int32_t  heading_motion;  // degrees * 1e5
        int32_t  heading_vehicle; // degrees * 1e5

        uint32_t horiz_acc;   // mm
        uint32_t vert_acc;    // mm
        uint32_t speed_acc;   // mm/s
        uint32_t heading_acc; // degrees * 1e5

        uint32_t reserved4;

        hnr_pvt_t() : msg_t( UBX_HNR, UBX_HNR_PVT, UBX_MSG_LEN(*this) ) {};
    } __attribute__((packed));

    struct cfg_nmea_t : msg_t {
        bool  always_output_pos  :1; // invalid or failed
        bool  output_invalid_pos :1;
        bool  output_invalid_time:1;
        bool  output_invalid_date:1;
        bool  use_GPS_only       :1;
        bool  output_heading     :1; // even if frozen
        bool  __not_used__       :2;
        enum {
            NMEA_V_2_1 = 0x21,
            NMEA_V_2_3 = 0x23,
            NMEA_V_4_0 = 0x40,  // Neo M8 family only
            NMEA_V_4_1 = 0x41   // Neo M8 family only
          }
            nmea_version : 8;
        enum {
            SV_PER_TALKERID_UNLIMITED =  0,
            SV_PER_TALKERID_8         =  8,
            SV_PER_TALKERID_12        = 12,
            SV_PER_TALKERID_16        = 16
          }
            num_sats_per_talker_id : 8;
        bool    compatibility_mode:1;
        bool    considering_mode  :1;
        bool    max_line_length_82:1;   // Neo M8 family only
        uint8_t __not_used_1__    :5;

        cfg_nmea_t() : msg_t( UBX_CFG, UBX_CFG_NMEA, UBX_MSG_LEN(*this) ) {};

      }  __attribute__((packed));

    struct cfg_nmea_v1_t : cfg_nmea_t {
        bool     filter_gps    :1;
        bool     filter_sbas   :1;
        uint8_t  __not_used_2__:2;
        bool     filter_qzss   :1;
        bool     filter_glonass:1;
        bool     filter_beidou :1;
        uint32_t __not_used_3__:25;

        bool proprietary_sat_numbering; // for non-NMEA satellites
        enum {
            MAIN_TALKER_ID_COMPUTED,
            MAIN_TALKER_ID_GP,
            MAIN_TALKER_ID_GL,
            MAIN_TALKER_ID_GN,
            MAIN_TALKER_ID_GA,
            MAIN_TALKER_ID_GB
          }
            main_talker_id : 8;
        bool gsv_uses_main_talker_id; // false means COMPUTED
        enum cfg_nmea_version_t {
            CFG_NMEA_V_0,  // length = 12
            CFG_NMEA_V_1   // length = 20 (default)
          }
            version : 8;

        //  Remaining fields are CFG_NMEA_V_1 only!
        char beidou_talker_id[2]; // NULs mean default
        uint8_t __reserved__[6];

        cfg_nmea_v1_t( cfg_nmea_version_t v = CFG_NMEA_V_1 )
          {
            version = v;
            if (version == CFG_NMEA_V_0)
              length = 12;
            else {
              length = 20;
              for (uint8_t i=0; i<8;) // fills 'reserved' too
                beidou_talker_id[i++] = 0;
            }
          };

      }  __attribute__((packed));

};

#endif // NMEAGPS_DERIVED_TYPES enabled

#endif