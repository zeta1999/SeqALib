
#ifndef SEQUENCE_ALIGNMENT_H
#define SEQUENCE_ALIGNMENT_H

#include "ArrayView.h"

#include <cassert>
#include <cmath>
#include <list>
#include <vector>
#include <algorithm>
#include <functional>
#include <limits.h> // INT_MIN

#define ScoreSystemType  int

// Store alignment result here
template<typename Ty, Ty Blank=Ty(0)>
class AlignedSequence {
public:

  class Entry {
  private:
    //TODO: change it for a vector<Ty> for Multi-Sequence Alignment
    std::pair<Ty,Ty> Pair;
    bool IsMatchingPair;
  public:
    Entry() { IsMatchingPair = false; }

    Entry(Ty V1, Ty V2) : Pair(V1,V2) { IsMatchingPair = !hasBlank(); }

    Entry(Ty V1, Ty V2, bool Matching) : Pair(V1,V2), IsMatchingPair(Matching) {}

    Ty get(size_t index) {
      assert((index==0 || index==1) && "Index out of bounds!");
      if (index==0) return Pair.first;
      else return Pair.second;
    }

    bool empty() { return (Pair.first==Blank && Pair.second==Blank); }
    bool hasBlank() { return (Pair.first==Blank || Pair.second==Blank); }

    bool match() { return IsMatchingPair; }
    bool mismatch() { return (!IsMatchingPair); }

    Ty getNonBlank() {
      if (Pair.first != Blank)
        return Pair.first;
      else
        return Pair.second;
    }

  };

  std::list< Entry > Data;

  AlignedSequence() {}

  AlignedSequence(const AlignedSequence<Ty> &Other) : Data(Other.Data) {}
  AlignedSequence(AlignedSequence<Ty> &&Other) : Data(std::move(Other.Data)) {}

  AlignedSequence<Ty> &operator=(const AlignedSequence<Ty> &Other) {
    Data = Other.Data;
    return (*this);
  }

  void append(const AlignedSequence<Ty> &Other) {
    Data.insert(Data.end(), Other.Data.begin(), Other.Data.end());
  }

  void splice(AlignedSequence<Ty> &Other) {
    Data.splice(Data.end(), Other.Data);
  }

  typename std::list< Entry >::iterator begin() { return Data.begin(); }
  typename std::list< Entry >::iterator end() { return Data.end(); }


};

class ScoringSystem {
  ScoreSystemType Gap;
  ScoreSystemType Match;
  ScoreSystemType Mismatch;
  bool AllowMismatch;
public:
  ScoringSystem(ScoreSystemType Gap, ScoreSystemType Match) {
    this->Gap = Gap;
    this->Match = Match;
    this->Mismatch = std::numeric_limits<ScoreSystemType>::min();
    this->AllowMismatch = false;
  }

  ScoringSystem(ScoreSystemType Gap, ScoreSystemType Match, ScoreSystemType Mismatch, bool AllowMismatch = true) {
    this->Gap = Gap;
    this->Match = Match;
    this->Mismatch = Mismatch;
    this->AllowMismatch = AllowMismatch;
  }

  bool getAllowMismatch() {
    return AllowMismatch;
  }

  ScoreSystemType getMismatchPenalty() {
    return Mismatch;
  }

  ScoreSystemType getGapPenalty() {
    return Gap;
  }

  ScoreSystemType getMatchProfit() {
    return Match;
  }
};

template<typename ContainerType, typename Ty=typename ContainerType::value_type, Ty Blank=Ty(0), typename MatchFnTy=std::function<bool(typename ContainerType::value_type,typename ContainerType::value_type)>>
class SequenceAligner {
private:
  ScoringSystem Scoring;
  MatchFnTy Match;
  //Ty Blank;

  ArrayView<ContainerType> Seq0;
  ArrayView<ContainerType> Seq1;

  AlignedSequence<Ty,Blank> Result;
public:

  using EntryType = typename AlignedSequence<Ty,Blank>::Entry;

  SequenceAligner(
    ScoringSystem Scoring,
    MatchFnTy Match,
    ContainerType &Seq0,
    ContainerType &Seq1) : Scoring(Scoring), Match(Match), Seq0(Seq0), Seq1(Seq1) {}  

  ScoringSystem &getScoring() { return Scoring; }

  bool match(Ty Val1, Ty Val2) {
    return Match(Val1,Val2);
  }

  MatchFnTy getMatchOperation() { return Match; }

  Ty getBlank() { return Blank; }

  AlignedSequence<Ty,Blank> &getResult() { return Result; }

  size_t getNumSequences() { return 2; }

  ArrayView<ContainerType> &getSequence(int idx) {
    assert((idx==0 || idx==1));
    return (idx==0)?Seq0:Seq1;
  }
};

#include "SANeedlemanWunsch.h"
#include "SAHirschberg.h"

#endif
