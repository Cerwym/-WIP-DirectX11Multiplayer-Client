#include "NWEntity.h"

NWEntity::NWEntity()
{
	m_online = false;
	m_movingForward = false;
	m_movingBackward = false;
	m_turningLeft = false;
	m_turningRight = false;
	m_EntityModel = 0;
	m_packetDifference = 0;
}

NWEntity::~NWEntity()
{
	S_DELETE(m_EntityModel);
}

bool NWEntity::Init(D3DSys* sys, char* modelName, WCHAR* textureName)
{
	m_D3DSys = sys;
	m_EntityModel = new D3DModel;
	if (!m_EntityModel->Init(m_D3DSys->GetDevice(), modelName, textureName, NULL))
		return false;
	return true;
}

void NWEntity::Render(D3DCamera* camera, D3DShaderManager* sm)
{
	if (m_EntityModel != 0)
	{
		m_EntityModel->TranslateTo(m_Position.x, m_Position.y, m_Position.z); // Fix this shit, it's ugly and shouldn't be used, add positional data and matrix to NWENTITY
		m_EntityModel->Render(m_D3DSys->GetDeviceContext(), sm, camera, NULL);
	}
	else
		MessageBox(NULL, L"EntityModel is null", L"No Model Loaded", MB_OK);
}

void NWEntity::UpdateState(char state)
{
	if(state == E_STATE_MOVING_FORWARD)       { m_movingForward = true; }
	if(state == E_STATE_NOT_MOVING_FORWARD)   { m_movingForward = false; }
	if(state == E_STATE_MOVING_BACKWARD)      { m_movingBackward = true; }
	if(state == E_STATE_NOT_MOVING_BACKWARD)  { m_movingBackward = false; }
	if(state == E_STATE_TURNING_LEFT)         { m_turningLeft = true; }
	if(state == E_STATE_NOT_TURNING_LEFT)     { m_turningLeft = false; }
	if(state == E_STATE_TURNING_RIGHT)        { m_turningRight = true; }
	if(state == E_STATE_NOT_TURNING_RIGHT)    { m_turningRight = false; }
}

void NWEntity::Update( float deltaTime )
{
	// If there has been no message from the server, begin predicting the position of this entity
	if (!m_messageRecieved)
	{
		oldState = newState;
		PredictPositionQuadratic(&oldState, &newState, deltaTime);
	}
	// Blend the two states together
	//kinematicState *state = BlendKinematicStateLinear(&oldState, &newState, 0.75f);
	//m_Position = state->position;
	//m_Velocity = state->velocity;
	//m_Acceleration = state->acceleration;
	BlendStateLinear(&newState, &oldState, &newState, deltaTime);
}

void NWEntity::PredictPositionQuadratic(kinematicState *old, kinematicState *predition, float elapsedSeconds)
{
	predition->position = old->position + (old->velocity * m_packetDifference) + (0.5f * old->acceleration * (m_packetDifference * m_packetDifference));
}

void NWEntity::BlendStateLinear(kinematicState *out, kinematicState * oldState, kinematicState *newState, float percentageToNew)
{
	float percentageToOld = 1.0f - percentageToNew;

	out->position = (percentageToOld * oldState->position) + (percentageToNew * newState->position);
	out->velocity = (percentageToOld * oldState->velocity) + (percentageToNew * newState->velocity);
	out->acceleration = (percentageToOld * oldState->acceleration) + (percentageToNew * newState->acceleration);
}

void NWEntity::UpdatePosition(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 velocity, XMFLOAT3 acceleration, unsigned long timestamp)
{
	newState.position = position;
	newState.velocity = velocity;
	newState.acceleration = acceleration;

	m_Position = position;
	m_Rotation = rotation;
	m_Acceleration = acceleration;
	m_packetDifference = (timestamp - m_packetDifference) / 1000;
}