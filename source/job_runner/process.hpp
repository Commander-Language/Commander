#ifndef COMMANDER_PROCESS_HPP
#define COMMANDER_PROCESS_HPP

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

namespace Process {

    /**
     * @brief Types of processes
     * @details External processes call programs in the system.
     *          Builtins are called within (see builtins folder)
     */
    enum ProcessType : u_int8_t {
        EXTERNAL,
        BUILTIN,
    };

    //  ==========================
    //  ||   Process Struct     ||
    //  ==========================

    /**
     * @brief Represents a process to execute
     */
    struct Process {
        /**
         * @brief Constructor for a pipeline of processes
         * @param processes - A list of processes to link
         * @details This links up the pipeline in the order given.
         *          Use other constructor to create these processes.
         */
        Process(std::vector<std::shared_ptr<Process>> processes);

        /**
         * @brief Constructor
         * @param isBackground - Sets if we run the process in the background (default to false)
         * @param isSave - Sets if we save the output to a tuple (default to false)
         */
        Process(std::vector<std::string> args, ProcessType type, bool isBackground = false, bool isSave = false);

        /**
         * @brief Destructor
         */
        ~Process() = default;

        /**
         * @brief Get the type of process
         * @return Type of process this is
         */
        [[nodiscard]] ProcessType getType() const;

        /**
         * @brief Get the name of the process
         * @return The name as a const char*
         */
        [[nodiscard]] const char* getName() const;

        /**
         * @brief The arguments for this process
         * @details The first argument should be the name of the process.
         */
        std::vector<std::string> args;

        /**
         * @brief The type of this process
         */
        ProcessType type;

        /**
         * @brief The name of this process
         */
        std::string processName;

        /**
         * @brief Is a background process or not
         */
        bool background;

        /**
         * @brief Save return information or not
         */
        bool saveInfo;

        /**
         * @brief A linked-list of processes, that represents a pipe.
         * @details Order of the linked-list is the order to execute and link pipes.
         */
        std::shared_ptr<Process> pipe = nullptr;

        /**
         * @brief The size of the pipeline
         */
        size_t pipeSize = 1;

        /**
         * @brief Is this process at the start of the pipeline
         */
        bool isFirst = false;

        /**
         * @brief Is this process at the end of the pipeline
         */
        bool isLast = false;
    };

    using ProcessPtr = std::shared_ptr<Process>;

}  // namespace Process

#endif  // COMMANDER_PROCESS_HPP
