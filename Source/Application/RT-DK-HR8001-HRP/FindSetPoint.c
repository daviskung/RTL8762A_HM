				if (FindSetPoint())

				{
					UINT16 rms = st2rms >> 9;

					SigMute = FALSE;		// see if operating point is still ok
					if ( findSetpointOnce )	findSetpointOnce = FALSE;
					
					if ( _currentDiagSettings.bit.autoGain )
					{
    						// adjust the AGC depending on avg RMS
    					if (rms < minRMS) GainUp();
                        
    					if (rms > maxRMS) GainDown();
                        
    					/**
    					 * Added safety to keep us from getting 'stuck' with bad gain 
    					 * steering 
    					 */
    					if ( ( (rms < minRMS) || (rms > maxRMS) )
    						 &&( (MAXGAIN == Gain) || (MINGAIN == Gain) ) )
    					{
    						if ( MAXGAIN == Gain )
    						{
    							while ( MINGAIN != Gain )
    							{
    								GainDown();
    							}
    						}
    						else
    						{
    							while ( MAXGAIN != Gain )
    							{
    								GainUp();
    							}
    						}
    					}
					}
				}