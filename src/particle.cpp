/************************************************************************/
/*                                                                      */
/* This file is part of VDrift.                                         */
/*                                                                      */
/* VDrift is free software: you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* VDrift is distributed in the hope that it will be useful,            */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with VDrift.  If not, see <http://www.gnu.org/licenses/>.      */
/*                                                                      */
/************************************************************************/

#include "particle.h"
#include "content/contentmanager.h"
#include "graphics/textureinfo.h"
#include "unittest.h"

bool PARTICLE_SYSTEM::Load(
	const std::string & texpath,
	const std::string & texname,
	int anisotropy,
	ContentManager & content)
{
	TEXTUREINFO texinfo;
	texinfo.anisotropy = anisotropy;
	return content.load(texture_atlas, texpath, texname, texinfo);
}

void PARTICLE_SYSTEM::Update(float dt, const QUATERNION <float> & camdir, const MATHVECTOR <float, 3> & campos)
{
	if (max_particles == 0)
		return;

	node.GetTransform().SetTranslation(campos);
	node.GetTransform().SetRotation(-camdir);

	//  update particles
	for (size_t i = 0; i < particles.size(); i++)
	{
		particles[i].Update(node, dt, camdir, campos);
	}

	// remove expired particles
	for (size_t i = 0; i < particles.size(); i++)
	{
		if (particles[i].Expired())
		{
			particles[i].Delete(node);

			//only bother to swap if it's not already at the end
			size_t last = particles.size() - 1;
			if (i != last)
				particles[i] = particles[last];

			particles.pop_back();
		}
	}
}

void PARTICLE_SYSTEM::AddParticle(
	const MATHVECTOR <float,3> & position,
	float newspeed,
	bool testonly)
{
	if (max_particles == 0)
		return;

	while (particles.size() >= max_particles)
		particles.pop_back();

	particles.push_back(PARTICLE(
		node, position, direction,
		speed_range.first + newspeed * (speed_range.second - speed_range.first),
		transparency_range.first + newspeed * (transparency_range.second - transparency_range.first),
		longevity_range.first + newspeed * (longevity_range.second - longevity_range.first),
		size_range.first + newspeed * (size_range.second - size_range.first),
		cur_texture_tile, texture_atlas));

	cur_texture_tile = (cur_texture_tile + 1) % texture_tiles;
}

void PARTICLE_SYSTEM::Clear()
{
	for (std::vector <PARTICLE>::iterator i = particles.begin(); i != particles.end(); ++i)
	{
		i->Delete(node);
	}
	particles.clear();
}

void PARTICLE_SYSTEM::SetParameters(
	int maxparticles,
	float transmin,
	float transmax,
	float longmin,
	float longmax,
	float speedmin,
	float speedmax,
	float sizemin,
	float sizemax,
	MATHVECTOR <float,3> newdir)
{
	max_particles = maxparticles < 0 ? 0 : (maxparticles > 1024 ? 1024 : maxparticles);
	particles.reserve(max_particles);

	transparency_range.first = transmin;
	transparency_range.second = transmax;
	longevity_range.first = longmin;
	longevity_range.second = longmax;
	speed_range.first = speedmin;
	speed_range.second = speedmax;
	size_range.first = sizemin;
	size_range.second = sizemax;
	direction = newdir;
}

QT_TEST(particle_test)
{
	std::stringstream out;
	PARTICLE_SYSTEM s;
	ContentManager c(out);
	s.SetParameters(4,1.0,1.0,0.5,1.0,1.0,1.0,1.0,1.0,MATHVECTOR<float,3>(0,1,0));
	s.Load(std::string(), std::string(), 0, c);

	//test basic particle management:  adding particles and letting them expire and get removed over time
	QT_CHECK_EQUAL(s.NumParticles(),0);
	s.AddParticle(MATHVECTOR<float,3>(0,0,0),0,true);
	QT_CHECK_EQUAL(s.NumParticles(),1);
	s.AddParticle(MATHVECTOR<float,3>(0,0,0),1,true);
	QT_CHECK_EQUAL(s.NumParticles(),2);
	QUATERNION <float> dir;
	MATHVECTOR <float, 3> pos;
	s.Update(0.45, dir, pos);
	QT_CHECK_EQUAL(s.NumParticles(),2);
	s.Update(0.1, dir, pos);
	QT_CHECK_EQUAL(s.NumParticles(),1);
	s.Update(0.5, dir, pos);
	QT_CHECK_EQUAL(s.NumParticles(),0);
}
