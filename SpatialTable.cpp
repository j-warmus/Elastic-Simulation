#include "SpatialTable.h"

int SpatialTable::hash(glm::vec3 position)
{
    int res;
    res = (((int)floor(position.x / d) * p1) ^ ((int)floor(position.y / d) * p2) ^ ((int)floor(position.x / d) * p3)) % m;
    return res;
}

std::vector<int>* SpatialTable::locate(glm::vec3 pos) {
    int h = hash(pos);
    if (table.find(h) == table.end() || table[h] == NULL)
        table[h] = new std::vector<int>;
    return table[h];
}

void SpatialTable::build(std::vector<FParticle> particles)
{
    for (unsigned i = 0; i < particles.size(); i++) {
        int h = hash(particles[i].position);
        if (table.find(h) == table.end() || table[h] == NULL)
            table[h] = new std::vector<int>;
        table[h]->push_back(i);
    }
}

void SpatialTable::clear() {
    table.clear();
}

void SpatialTable::getneighbors(FParticle particle, float support, std::vector<FParticle> particles)
{
    // no way for a particle to have influence more than a support away.
    std::vector<int> candidates;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            for (int k = -1; k <= 1; k++) {
                glm::vec3 newloc = particle.position + glm::vec3(i * support, j * support, k * support);
                std::vector<int>* b = locate(newloc);
                candidates.insert(candidates.end(), b->begin(), b->end());
            }
        }
    }

    for (int i = 0; i < candidates.size(); i++) {
        if (&particles[i] != &particle) {
            if (glm::length(particle.position - particles[i].position) < support) {
                particle.neighbors.push_back(i);
            }
        }
    }

}
