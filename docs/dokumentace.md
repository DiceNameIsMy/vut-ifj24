# 1. Title Page

Team xpopov10, variant vv-BVS.

Team Members:
- Albert Popov TODO <- Team leader
- Nurdaulet Turar (xturarn00) 
- Oleg TODO
- Ivan Savin TODO

Points Allocation: TODO Breakdown of points distribution among team members.

Implemented Extensions: TODO List the identifiers of all implemented extensions.

# 2. Work Distribution

Member Contributions: Describe the contributions of each team member to the project.

Albert: TODO

Nurdaulet:
- Lead the team & enabled them to make work without bothering with automation.
- Lexer (~20%)
- Target code generation (100%)
- Documenatation (TODO%)
- Repository automation (100%)
  - Setup Continuous Integration with tests
  - Scripts to run tests, to add a comment line with authors, etc.
  - Script to archive the project for submission

Oleg: TODO

Ivan: TODO

Justification of Unequal Distribution: Explain any deviations from an even distribution of points.

# 3. Lexical Analyzer Specification

Finite State Machine Diagram: Provide the diagram of the finite automaton used to specify the lexical analyzer.

# 4. Syntax Analyzer Specification

LL Grammar: Present the grammar used in the implementation.

LL Parsing Table: Include the parsing table for the LL grammar.

Precedence Table: Provide the precedence table used in the syntax analyzer.

# 5. Implementation Structure

Solution Overview: Briefly describe the structure of your implementation.

Separated into 3 main modules:
- lexer: Converts the `Source Code` to `Tokens`
- parser: Analyzes the `Tokens` with Syntax & Sematic rules, then outputs an `Abstract Syntax Tree(AST)`
- target_gen: Generates from `AST` the `Target Code`

File Details: List the file names and describe their contents, highlighting where the mandatory methods for the compiler are implemented.

## 5.1 Lexer
TODO

## 5.2 Parser
TODO

## 5.3 Target Generation

- `id_indexer.h`: Used to add a unique index for labels and each function scope, in order to avoid label/variable names collisions.
- `instructions.h`: Module that encapsulates the target code's instructions construction.
- `target_func_context.h`: Used to store all variables that can be used within a function context. They are then added at the beggining of the generated function because of the implementation
- `target_gen.h`: An entrypoint to the target code generations

## 5.4 Other

- logging.h
- dynBuffer.h
- symtable.h
- TODO

# 6. Development Process

Initially, we've tried assigning 2 people for each component: lexer, syntax and semantic analysys, and target generation. This idea did incentivise each member to be more aware not only of the part he's implementing, but also of next of previous component, which in theory would make the development process smoother by encouraging communications, dicussions, and more careful planning.

We've created a common chat where we were asking questions, planning next steps, meetings, coordinating the interfaces between components, and discussing issues we've stubmled upon during the implementation.

Code is in a provate GitHub repository. Development is done in feature branches with a mandatory review and approval process before merging to the master branch.

CI workflow to run tests is configured. It is ran on pull request creation, but can be ran manually too.

## 6.2 Challenges and Solutions:

We (Ivan and Nurdaulet) overlooked the requirement for lexer to be based on FSM when implementing it. When we finally did, it did use an FSM, doesn't mean it's based on it, since most of the code wasn't using this concept.

After target code generation was almost completed, Nurdaulet has realized that we could've used a polish notation to simplify expressions generation on numbers, but due to deadline coming near, It's been decided to keep it as is.

## 6.3 Code Quality

To ensure code quality:
- Everything was developed in GitHub.
- No code style rules were enforced.
- Changes were integratable only after a pull request that was reviewed and approved by another team member.
- A Continuous Integration (CI) pipeline was setup that builds, runs, & tests the project before and after merging the pull request
- Scripts to run tests locally were added
- First, I've (Nurdaulet) added per-component testing framework that was then used for testing 
  - lexer
  - data structures
  - parser's syntax analysis
- Later on, when we started combining our components, a python script to run a whole compiler with test cases was added.
- Before final submission, a script to test, build, archive, and run ./is_it_okay.sh script was added. 
