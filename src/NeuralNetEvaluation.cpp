#include "NeuralNetEvaluation.h"

NeuralNetEvaluation::NeuralNetEvaluation() {
	this->model = new Model(Util::getExePath() + "/ann/model/newnorm.pb");
	this->model->restore(Util::getExePath() + "/ann/model/newnorm.ckpt");
	this->input = new Tensor(*this->model, "inputoud");
	this->output = new Tensor(*this->model, "outputoud");
	this->featureGenerator = new FeatureGenerator();
	
	// TODO: hier zo verder, Move gen verwacht ook een board.
	// Kan alles static (feature, movegen, neuralnet,) zodat je geen instance hoeft aan te mkaen?
	// Hoe doe je dan je model inladen en in memory houden(global?, of enkel evaluation static en instance niet?)
	// wss niet mogelijk om movegen static te maken ivm resetten van board state. Move gen -> eval -> movgen requires 2 resets
	///  Movegen object maken met replaceable board.
	//this->moveGeneration = new MoveGeneration(//hier moet een board)
}

NeuralNetEvaluation::~NeuralNetEvaluation() {
	delete this->model;
	delete this->input;
	delete this->output;
	delete this->featureGenerator;
}

int NeuralNetEvaluation::Evaluate(Board* board)
{
	std::vector<double> featureVector = this->featureGenerator->generate(board);
	std::vector<float> featureVectorFloat(featureVector.begin(), featureVector.end());
	
	this->input->set_data(featureVectorFloat);

	this->model->run(this->input, this->output);
	// Scale output back to -1500 - 1500
	int evaluation = static_cast<int>(this->output->get_data<float>()[0] * 1500.0);

	return (board->turn == White ? evaluation : -evaluation);
}