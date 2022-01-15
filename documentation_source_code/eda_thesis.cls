%%
%% This is file `eda_thesis.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% eda_thesis.dtx  (with options: `package')
%%
%% Please consult the documentation provided by eda_thesis.pdf
%%
%% If you have to make changes, do not edit eda_thesis.sty but make your
%% changes in eda_thesis.dtx. Do not forget to commit your changes to the
%% institute.
%%

 \NeedsTeXFormat{LaTeX2e}
 \ProvidesClass{eda_thesis}[2010/09/22 v0.9 mce]


\newcommand{\edathesis@thesisnamede}{tbd Select thesis type}
\newcommand{\edathesis@thesisnameen}{tbd Select thesis type}
\def\edathesis@topicIssued{}
\def\edathesis@submission{}
\def\edathesis@supervisor{}
\def\edathesis@profsupervisor{}
\def\edathesis@address{}
\def\edathesis@period{}
\def\edathesis@phd@chair{\makebox[7cm]{\dotfill}}
\def\edathesis@phd@examA{\makebox[7cm]{\dotfill}}
\def\edathesis@phd@examB{\makebox[7cm]{\dotfill}}
\def\edathesis@phd@examC{\makebox[7cm]{\dotfill}}
\def\edathesis@phd@accept{\makebox[3cm]{\dotfill\ }}
\newif\ifedathesis@outertitle
\edathesis@outertitlefalse
\newif\ifedathesis@twoside
\newif\ifedathesis@phd
\DeclareOption{msc}{
    \renewcommand{\edathesis@thesisnamede}{Arbeitsbericht 2019/2020}
\renewcommand{\edathesis@thesisnameen}{Documentation}
    %\renewcommand{\edathesis@period}{??.??.???? - ??.??.????}
}
\DeclareOption{bsc}{
    \renewcommand{\edathesis@thesisnamede}{Bachelorarbeit}
    \renewcommand{\edathesis@thesisnameen}{Bachelor's Thesis}
    \renewcommand{\edathesis@profsupervisor}{N.N.}
    \renewcommand{\edathesis@topicIssued}{??.??.????}
    \renewcommand{\edathesis@submission}{??.??.????}
}
\DeclareOption{da}{
    \renewcommand{\edathesis@thesisnamede}{Diplomarbeit}
    \renewcommand{\edathesis@thesisnameen}{Diploma Thesis}
    \renewcommand{\edathesis@profsupervisor}{N.N.}
    \renewcommand{\edathesis@topicIssued}{??.??.????}
    \renewcommand{\edathesis@submission}{??.??.????}
}
\DeclareOption{stua}{
    \renewcommand{\edathesis@thesisnamede}{Studienarbeit}
    \renewcommand{\edathesis@thesisnameen}{Seminar Paper}
    \renewcommand{\edathesis@profsupervisor}{N.N.}
    \renewcommand{\edathesis@topicIssued}{??.??.????}
    \renewcommand{\edathesis@submission}{??.??.????}
}
\DeclareOption{ingp}{
    \renewcommand{\edathesis@thesisnamede}{Bericht zur Ingenieurpraxis}
    \renewcommand{\edathesis@thesisnameen}{Engineering Internship Report}
    \renewcommand{\edathesis@profsupervisor}{N.N.}
    \renewcommand{\edathesis@period}{??.??.???? - ??.??.????}
}
\DeclareOption{fp}{
    \renewcommand{\edathesis@thesisnamede}{Bericht zur Forschungspraxis}
    \renewcommand{\edathesis@thesisnameen}{Research Internship Report}
    \renewcommand{\edathesis@profsupervisor}{N.N.}
    \renewcommand{\edathesis@period}{??.??.???? - ??.??.????}
}
\DeclareOption{ints}{
    \renewcommand{\edathesis@thesisnamede}{Praktikumsbericht}
    \renewcommand{\edathesis@thesisnameen}{Internship Report}
    \renewcommand{\edathesis@period}{??.??.???? - ??.??.????}
}
\DeclareOption{phd}{
    \edathesis@phdtrue
    \renewcommand{\edathesis@submission}{\makebox[3cm]{\dotfill\ }}
}
\DeclareOption{outertitle}{
    \edathesis@outertitletrue
}
\DeclareOption{twoside}{
    \edathesis@twosidetrue
}
\DeclareOption{oneside}{
    \edathesis@twosidefalse
}
\DeclareOption*{%
    \ClassWarning{eda_thesis}{Unknown option `\CurrentOption'; Passing down to scrreprt}%
    \PassOptionsToClass{\CurrentOption}{scrreprt}
}
\ExecuteOptions{twoside}
\ProcessOptions
\LoadClass[openright,abstracton,parskip,bibtotoc,pointednumbers,smallheadings,DIV13]{scrreprt}

\RequirePackage{tumcdTitlePage}
\pagestyle{headings}

\KOMAoptions{headinclude=true,fontsize=11pt}
\ifedathesis@twoside
    \KOMAoptions{twoside=true}
\fi
\recalctypearea
\DeclareGraphicsRule{.fig.pdf}{pdf}{*}{}
\DeclareGraphicsRule{.svg.pdf}{pdf}{*}{}
\let\edathesis@tumcd@maketitle\maketitle
\ifedathesis@phd
  \renewcommand{\maketitle}{
  \ifedathesis@outertitle
    \edathesis@tumcd@maketitle
    \cleardoubleemptypage
  \fi
  \titletext{
  Vollst\"andiger Abdruck der von der Fakult\"at f\"ur Elektro- und Informationstechnik zur Erlangung des akademischen Grades eines\\[5mm]
  \textbf{Doktor-Ingenieurs (Dr.-Ing.)}\\[5mm]
  genehmigten Dissertation.
  \vspace{10mm}\\
  \noindent
  \begin{tabular}{lr@{ }l}
    \textbf{Vorsitzender:} & & \ifx\edathesis@phd@chair\empty %
    \makebox[6cm]{\dotfill}
    \else
      \edathesis@phd@chair
    \fi\\[5mm]
    \textbf{Pr\"ufer der Dissertation:} & 1. & \edathesis@phd@examA\\[5mm]
    & 2. & \edathesis@phd@examB\\[5mm]
    & 3. & \edathesis@phd@examC\\[5mm]
  \end{tabular}
  \vspace{10mm}\\
  Die Dissertation wurde am \edathesis@submission\ bei der Technischen Universit\"at M\"unchen eingereicht und durch die Fakult\"at f\"ur Elektro- und Informationstechnik am \edathesis@phd@accept\ angenommen.
  }
  \edathesis@tumcd@maketitle
  \cleardoubleemptypage
  }
\else
  \subtitle{\tumcd@lang{\edathesis@thesisnamede}{\edathesis@thesisnameen}}
  \renewcommand{\maketitle}{
    \ifedathesis@outertitle
        \edathesis@tumcd@maketitle
        \cleardoubleemptypage
    \fi
    \titletext{
        \setcounter{page}{1}
        \vspace*{2cm}\\
        \hspace*{-2.5mm}\begin{tabular}{ll}
            \if\edathesis@supervisor\empty \else \tumcd@lang{Betreuer}{Advisor}: & \edathesis@supervisor \\\fi
            \if\edathesis@profsupervisor\empty \else \tumcd@lang{Betreuender Hochschullehrer}{Advising Professor}: & \edathesis@profsupervisor\\\fi
            \ifx\edathesis@topicIssued\empty \else \tumcd@lang{Geplanter Durchf\"uhrungszeitraum}{Topic issued}: &\edathesis@topicIssued\\\fi
            \ifx\edathesis@submission\empty \else  \tumcd@lang{Abgabedatum}{Date of submission}: &\edathesis@submission\\\fi
            \ifx\edathesis@period\empty \else \tumcd@lang{Geplanter Durchführungszeitraum}{Working period}: &\edathesis@period\\\fi
        \end{tabular}
        \vspace*{2cm}\\
        \typeout{addr '\edathesis@address' empty '\empty'}
        \if\edathesis@address\empty \else
            \noindent\edathesis@address\\[1cm]
        \fi
    }
    \edathesis@tumcd@maketitle
    \cleardoubleemptypage
  }
\fi
\newcommand{\issueDate}[3]{\renewcommand{\edathesis@topicIssued}{#1.#2.#3}}
\newcommand{\submissionDate}[3]{\renewcommand{\edathesis@submission}{#1.#2.#3}}
\newcommand{\supervisor}[2]{
    \renewcommand{\edathesis@supervisor}{#1}
    \renewcommand{\edathesis@profsupervisor}{#2}
}

\newcommand{\phdChair}[1]{\def\edathesis@phd@chair{#1}}
\newcommand{\phdExamA}[1]{\def\edathesis@phd@examA{#1}}
\newcommand{\phdExamB}[1]{\def\edathesis@phd@examB{#1}}
\newcommand{\phdExamC}[1]{\def\edathesis@phd@examC{#1}}
\newcommand{\phdAcceptDate}[3]{\def\edathesis@phd@accept{#1.#2.#3}}

\newcommand{\address}[1]{\renewcommand{\edathesis@address}{#1}}
\newcommand{\period}[1]{\renewcommand{\edathesis@period}{#1}}
\AtBeginDocument{
    \abovedisplayskip=16.5pt plus 3pt minus 6pt
    \abovedisplayshortskip=11pt plus 3pt minus 6pt
    \belowdisplayskip=16.5pt plus 3pt minus 6pt
    \belowdisplayshortskip=16.5pt plus 3pt minus 6pt
}
\endinput
%%
%% End of file `eda_thesis.cls'.
