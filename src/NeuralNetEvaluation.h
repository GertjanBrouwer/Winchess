#pragma once
#include "Evaluation.h"
#include "Board.h"
#include "FeatureGenerator.h"
#include "Model.h"
#include "Tensor.h"

class NeuralNetEvaluation : public Evaluation
{
private:
	FeatureGenerator* featureGenerator;
	Model* model;
	Tensor* input;
	Tensor* output;

public:
	NeuralNetEvaluation();
	~NeuralNetEvaluation();
	
	int Evaluate(Board* board);
};