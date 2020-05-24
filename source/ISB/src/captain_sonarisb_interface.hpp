
#ifndef CAPTAIN_SONARISB_INTERFACE_H
#define	CAPTAIN_SONARISB_INTERFACE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define CPT_MSG_DEPTH 1

// For debugging
#define CPT_MSG_SQP 2   // Send square pulse
#define CPT_MSG_DCV 3   // Download correlation vector (last echo)
#define CPT_MSG_DTV 4   // Download time vector (last echo)
#define CPT_MSG_GSY 5   // Geo-sync (time and position from onboard GPS)

#ifdef	__cplusplus
}
#endif
#endif	/* CAPTAIN_SONARISB_INTERFACE_H */

