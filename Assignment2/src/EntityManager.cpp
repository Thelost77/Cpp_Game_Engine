#include "EntityManager.h"

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
    // add entities from the add vector to the main vector
    for (auto &entity : m_entitiesToAdd)
    {
        m_entities.push_back(entity);
        m_entityMap[entity->tag()].push_back(entity);
    }
    m_entitiesToAdd.clear();

    // remove dead entities from the vector of all entities
    removeDeadEntities(m_entities);

    // remove dead entities from each vector in the entity map
    for (auto &[tag, entityVec] : m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec &vec)
{
    // TODO: remove all dead entities from the input vector
    // this is called by the update() function
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    auto entity = std::make_shared<Entity>(++m_totalEntities, tag);
    m_entitiesToAdd.push_back(entity);
    return entity;
}

const EntityVec &EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec &EntityManager::getEntities(const std::string &tag)
{
    return m_entityMap[tag];
}

const std::map<std::string, EntityVec> &EntityManager::getEntityMap()
{

    return m_entityMap;
}