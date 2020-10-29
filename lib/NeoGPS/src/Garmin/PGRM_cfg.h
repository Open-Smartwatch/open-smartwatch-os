#ifndef PGRM_CFG_H
#define PGRM_CFG_H

//------------------------------------------------------------
// Enable/disable the parsing of specific proprietary NMEA sentences.
//
// Configuring out a sentence prevents its fields from being parsed.
// However, the sentence type may still be recognized by /decode/ and 
// stored in member /nmeaMessage/.  No valid flags would be available.

//#define GARMINGPS_PARSE_F

#endif