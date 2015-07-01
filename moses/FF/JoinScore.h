#pragma once

#include <boost/unordered_map.hpp>
#include <string>
#include "StatefulFeatureFunction.h"
#include "FFState.h"
#include "JoinScore/TrieSearch.h"

namespace Moses
{


class JoinScore : public StatefulFeatureFunction
{
////////////////////////////////////////////////////////////////
  class Node
  {
  public:
    bool isAWord;

    Node();
    Node *Insert(const std::string &tok);
    const Node *Find(const std::string &tok) const;

  protected:
    typedef boost::unordered_map<char, Node*> Children;
    Children m_children;

    Node *Insert(const std::string &tok, size_t pos);
    Node *GetOrCreateNode(char c);

    const Node *Find(const std::string &tok, size_t pos) const;
  };

////////////////////////////////////////////////////////////////

  class JoinScoreState : public FFState
  {
    //std::string m_concat;
    Phrase m_morphemes;
    size_t m_juncture;
      //None  =  0,
      //Left  =  1,
      //Right =  2
    
  public:
    JoinScoreState()
    :m_juncture(0)
    {}
    
    JoinScoreState(const Phrase &morphemes, size_t juncture)
    :m_morphemes(morphemes)
    ,m_juncture(juncture)
    {
    }

    int Compare(const FFState& other) const;
    
    const Phrase &GetMorphemes() const {
      return m_morphemes;
    }

    int GetJuncture() const {
      return m_juncture;
    }

  };
////////////////////////////////////////////////////////////////

public:
  JoinScore(const std::string &line);

  virtual void Load();

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }
  virtual const FFState* EmptyHypothesisState(const InputType &input) const {
    return new JoinScoreState();
  }

  void EvaluateInIsolation(const Phrase &source
                           , const TargetPhrase &targetPhrase
                           , ScoreComponentCollection &scoreBreakdown
                           , ScoreComponentCollection &estimatedFutureScore) const;
  void EvaluateWithSourceContext(const InputType &input
                                 , const InputPath &inputPath
                                 , const TargetPhrase &targetPhrase
                                 , const StackVec *stackVec
                                 , ScoreComponentCollection &scoreBreakdown
                                 , ScoreComponentCollection *estimatedFutureScore = NULL) const;

  void EvaluateTranslationOptionListWithSourceContext(const InputType &input
      , const TranslationOptionList &translationOptionList) const;

  FFState* EvaluateWhenApplied(
    const Hypothesis& cur_hypo,
    const FFState* prev_state,
    ScoreComponentCollection* accumulator) const;
  FFState* EvaluateWhenApplied(
    const ChartHypothesis& /* cur_hypo */,
    int /* featureID - used to index the state in the previous hypotheses */,
    ScoreComponentCollection* accumulator) const;

  void SetParameter(const std::string& key, const std::string& value);
    
protected:
  bool m_scoreRealWords, m_scoreNumCompounds, 
        m_scoreInvalidJoins, m_scoreCompoundOOV,
        m_scoreCompoundWord, m_scorePartialCompound
        ;
  int m_maxMorphemeState;
  float m_multiplier;
  std::string m_vocabPath;

  //Node m_vocabRoot;
  TrieSearch<bool> m_trieSearch;
  
  int GetJuncture(const Word &word) const;
  void CalcScores(size_t &numWord, size_t&numCompoundWord, 
                          size_t &numInvalidJoin, float &compoundWordScore,
                          Phrase &morphemes, const Node *&node, 
                          const Word *morpheme,
                          int prevJuncture, int currJuncture) const;
  float CalcMorphemeScore(const Phrase &morphemes, bool wholeWord) const;

  void AddMorphemeToState(Phrase &morphemes, const Word *morpheme) const;
  float CalcScore(float count) const;
};


}
