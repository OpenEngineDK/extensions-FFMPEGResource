// Movie resource interface.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _I_MOVIE_RESOURCE_H_
#define _I_MOVIE_RESOURCE_H_

#include <Core/IModule.h>
#include <Resources/ITextureResource.h>

namespace OpenEngine {
namespace Resources {

using OpenEngine::Core::IModule;

/**
 * Movie resource interface.
 *
 * @class IMovieResource IMovieResource.h Resources/IMovieResource.h
 */
class IMovieResource : public IModule, public ITextureResource {
public:

    /**
     * Pause the movie.
     */
    virtual void Pause(bool pause) = 0;

    /**
     * Ask if the movie has ended
     *
     * @return true if the movie has ended
     */
    virtual bool Ended() = 0;

    /**
     * restart the movie from the beginning
     */
    virtual void Restart() = 0;

    /**
     * Get the original height of the movie
     *
     * @return height in pixels
     */
    virtual int GetMovieHeight() = 0;

    /**
     * Get the original width of the movie
     *
     * @return width in pixels
     */
    virtual int GetMovieWidth() = 0;	
};

/**
 * Movie resource smart pointer.
 */
typedef boost::shared_ptr<IMovieResource> IMovieResourcePtr;

} // NS Resources
} // NS OpenEngine

#endif // _I_MOVIE_RESOURCE_H_
