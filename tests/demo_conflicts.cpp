/*
 * Copyright (c) 2025 HobbyHacker
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iostream>
#include <cstdlib>   // Contains std::abs
#include <cmath>     // Contains std::abs for floating point

int main() {
    std::cout << "=== Standard Library Math Functions ===" << std::endl;
    
    // These are from the C standard library (available globally and in std::)
    std::cout << "std::abs(-42) = " << std::abs(-42) << std::endl;
    std::cout << "std::abs(-42L) = " << std::abs(-42L) << std::endl;
    std::cout << "std::abs(-3.14f) = " << std::abs(-3.14f) << std::endl;
    std::cout << "std::abs(-3.14) = " << std::abs(-3.14) << std::endl;
    
    // These are also available in global scope (without std::)
    std::cout << "abs(-42) = " << abs(-42) << std::endl;
    
    std::cout << "\n=== Functions NOT in Standard Library ===" << std::endl;
    std::cout << "constrain() - Arduino specific, not in C++ standard" << std::endl;
    std::cout << "map() - Arduino specific, not in C++ standard" << std::endl;
    
    std::cout << "\n=== Why We Use Namespaces ===" << std::endl;
    std::cout << "If we put Arduino abs() in global scope:" << std::endl;
    std::cout << "- Conflicts with ::abs() from <cstdlib>" << std::endl;
    std::cout << "- Ambiguous function calls" << std::endl;
    std::cout << "- Compilation errors" << std::endl;
    
    return 0;
}