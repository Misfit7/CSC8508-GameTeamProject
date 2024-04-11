#pragma once
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"

#include "GameWorld.h"

namespace NCL {
    class Maths::Vector2;
    class Maths::Vector3;
    class Maths::Vector4;

    namespace CSC8503 {
        // Represents a single particle and its state
        struct Particle {
            Vector3 Position, Velocity;
            Vector4 Color;
            float   Life;

            Particle() : Position(0.0f, 0.0f, 0.0f), Velocity(0.0f, 0.0f, 0.0f),
                Color(1.0f, 1.0f, 1.0f, 1.0f), Life(0.0f) { }
        };

        class ParticleGenerator
        {
        public:
            // constructor
            ParticleGenerator(OGLShader* shader, unsigned int amount);
            // update all particles
            void Update(float dt, GameObject* object, unsigned int newParticles, Vector3 offset = Vector3(0.0f, 0.0f, 0.0f));
            // render all particles
            std::vector<Particle> particles;
            unsigned int VAO;
            unsigned int VBO;
            GLuint texture;
            // initializes buffer and vertex attributes
            void init();
        private:
            // state
            unsigned int amount;
            // render state
            OGLShader* shader;

            // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
            unsigned int firstUnusedParticle();
            // respawns particle
            void respawnParticle(Particle& particle, GameObject* object, Vector3 offset = Vector3(0.0f, 0.0f, 0.0f));
        };
    }
}