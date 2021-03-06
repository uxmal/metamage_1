/*
	Controls.hh
	-----------
*/

#ifndef CONTROLS_HH
#define CONTROLS_HH

struct GrafPort;
struct MacRegion;
struct Point;
struct Rect;
struct ControlRecord;

typedef pascal void (*ControlActionProcPtr)( ControlRecord**, short );

pascal void UpdateControls_patch( GrafPort* window, MacRegion** updateRgn );

pascal ControlRecord** NewControl_patch( GrafPort*             window,
                                         const Rect*           bounds,
                                         const unsigned char*  title,
                                         short                 visible,
                                         short                 value,
                                         short                 min,
                                         short                 max,
                                         short                 procID,
                                         long                  refCon );

pascal void DisposeControl_patch( ControlRecord** control );

pascal void KillControls_patch( GrafPort* window );

pascal short TrackControl_patch( ControlRecord**  control,
                                 Point            start,
                                 pascal void    (*action)() );

pascal void DrawControls_patch( GrafPort* window );

pascal void HiliteControl_patch( ControlRecord** control, short hiliteState );

pascal short FindControl_patch( Point             where,
                                GrafPort*         window,
                                ControlRecord***  which );

pascal void SetCtlValue_patch( ControlRecord** control, short value );

pascal short GetCtlValue_patch( ControlRecord** control );

pascal void SetMinCtl_patch( ControlRecord** control, short min );
pascal void SetMaxCtl_patch( ControlRecord** control, short max );

pascal short GetMinCtl_patch( ControlRecord** control );
pascal short GetMaxCtl_patch( ControlRecord** control );

pascal void SetCRefCon_patch( ControlRecord** control, long refCon );
pascal long GetCRefCon_patch( ControlRecord** control );

pascal void SetCtlAction_patch( ControlRecord** control, ControlActionProcPtr action );

pascal ControlActionProcPtr GetCtlAction_patch( ControlRecord** control );

#endif
