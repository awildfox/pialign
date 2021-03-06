#ifndef PARSE_CHART_H__
#define PARSE_CHART_H__

#include <vector>
#include "pialign/definitions.h"

namespace pialign {

class ParseChart : public std::vector<Prob> {

private:

    // a sorting for probability pairs
    static bool moreProb(const ProbSpan & a, const ProbSpan & b) {
        return a.first > b.first;
    }

protected:

    int eMultiplier_;
    Agendas agendas_; // the current set of bucketed agendas
    int debug_;
    int eLen_, fLen_;

public:

    ParseChart() : std::vector<Prob>(), debug_(0) { }

    // chart access
    inline int findChartPosition(const Span & s) const {
        return findChartPosition(s.es,s.ee,s.fs,s.fe);
    }
    inline int findChartPosition(int s, int t, int u, int v) const {
        return (t*(t+1)/2+s)*eMultiplier_+v*(v+1)/2+u;
    } 

    inline void addToChart(int s, int t, int u, int v, Prob p, int idx, int len) {
#ifdef DEBUG_ON
        if(len == 0)
            throw std::runtime_error("ParseChart attempted to add an empty span");
        if(p != p)
            throw std::runtime_error("ParseChart attempted to add nan value");
#endif
        if((*this)[idx] <= NEG_INFINITY) {
            agendas_[len-1].push_back(Span(s,t,u,v));
            (*this)[idx] = p;
        }
        else
            (*this)[idx] = addLogProbs((*this)[idx],p);
#ifdef DEBUG_ON
        if( p <= NEG_INFINITY || p > 0 )
            throw std::runtime_error("ParseChart Attempted to add illegal probability");
        if(debug_)
            std::cerr << "addToChart(Span("<<s<<","<<t<<","<<u<<","<<v<<"), "<<p<<") == " << (*this)[idx] <<std::endl;
#endif

    }

    inline void addToChart(int s, int t, int u, int v, Prob p) {
        addToChart(s,t,u,v,p,findChartPosition(s,t,u,v),v - u + t - s);
    }
    inline void addToChart(const Span & s, Prob p) {
        addToChart(s.es,s.ee,s.fs,s.fe,p);
    }

    inline Prob getFromChart(int s, int t, int u, int v) const {
        return (*this)[findChartPosition(s,t,u,v)];
    }
    inline Prob getFromChart(const Span & s) const {
        // cerr << "getFromChart("<<s.es<<","<<s.ee<<","<<s.fs<<","<<s.fe<<") == "<<(*this)[findChartPosition(s)]<<endl;
        return (*this)[findChartPosition(s)];
    }
    inline void removeFromChart(int s, int t, int u, int v) {
        removeFromChart(Span(s,t,u,v));
    }
    inline void removeFromChart(const Span & s) {
        (*this)[findChartPosition(s)] = NEG_INFINITY;
    }

    // initialize the parse chart to the appropriate size
    void initialize(int eLen, int fLen);

    // trim and return an agenda
    ProbSpanSet getTrimmedAgenda(int l, int histWidth, Prob probWidth);

    void setDebug(int debug) { debug_ = debug; }

};

}

#endif
