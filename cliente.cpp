#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "notas.grpc.pb.h"

class NotasClient {
public:
    NotasClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(gerencia_notas::GerenciadorNotas::NewStub(channel)) {}

    void testarTudo() {
        //Adicionar Notas
        std::cout << "\n1. Adicionando notas..." << std::endl;
        adicionarNota("789", "FIS0001", 2025, 1, 10.0);
        adicionarNota("789", "MAT0001", 2025, 1, 9.0);
        adicionarNota("101", "FIS0001", 2025, 1, 6.5);

        std::cout << "\n2. Consultando nota de FIS0001 para RA 789..." << std::endl;
        consultarNota("789", "FIS0001");

        std::cout << "\n3. Alterando nota de MAT0001 para RA 789 para 9.5..." << std::endl;
        alterarNota("789", "MAT0001", 2025, 1, 9.5);

        std::cout << "\n4. Calculando média para RA 789..." << std::endl;
        calcularMedia("789");

        std::cout << "\n5. DESAFIO: Listando todas as notas do RA 789 via streaming..." << std::endl;
        listarNotasAluno("789");
    }

private:
    void adicionarNota(const std::string& ra, const std::string& cod, int ano, int semestre, float nota_val) {
        gerencia_notas::AdicionaNotaRequest request;
        request.set_ra(ra);
        request.set_cod_disciplina(cod);
        request.set_ano(ano);
        request.set_semestre(semestre);
        request.set_nota(nota_val);

        gerencia_notas::StatusResponse reply;
        grpc::ClientContext context;
        grpc::Status status = stub_->AdicionarNota(&context, request, &reply);

        if (status.ok()) {
            std::cout << "  - Resposta: " << reply.msg() << std::endl;
        } else {
            std::cout << "  - Erro RPC: " << status.error_message() << std::endl;
        }
    }

    void alterarNota(const std::string& ra, const std::string& cod, int ano, int semestre, float nota_val) {
        gerencia_notas::AdicionaNotaRequest request;
        request.set_ra(ra);
        request.set_cod_disciplina(cod);
        request.set_ano(ano);
        request.set_semestre(semestre);
        request.set_nota(nota_val);

        gerencia_notas::StatusResponse reply;
        grpc::ClientContext context;
        grpc::Status status = stub_->AlterarNota(&context, request, &reply);
        if (status.ok()) {
            std::cout << "  - Resposta: " << reply.msg() << std::endl;
        } else {
            std::cout << "  - Erro RPC: " << status.error_message() << std::endl;
        }
    }

    void consultarNota(const std::string& ra, const std::string& cod) {
        gerencia_notas::AlunoDisciplinaRequest request;
        request.set_ra(ra);
        request.set_cod_disciplina(cod);

        gerencia_notas::ConsultaNotaResponse reply;
        grpc::ClientContext context;
        grpc::Status status = stub_->ConsultarNota(&context, request, &reply);
        if (status.ok()) {
            if (reply.sucesso()) {
                const auto& n = reply.nota();
                std::cout << "  - Nota encontrada: RA=" << n.ra()
                          << " Disc=" << n.cod_disciplina()
                          << " Ano=" << n.ano()
                          << " Sem=" << n.semestre()
                          << " Nota=" << n.nota() << std::endl;
            } else {
                std::cout << "  - Consulta falhou: " << reply.msg_erro() << std::endl;
            }
        } else {
            std::cout << "  - Erro RPC: " << status.error_message() << std::endl;
        }
    }

    void calcularMedia(const std::string& ra) {
        gerencia_notas::AlunoRequest request;
        request.set_ra(ra);

        gerencia_notas::MediaResponse reply;
        grpc::ClientContext context;
        grpc::Status status = stub_->CalcularMedia(&context, request, &reply);
        if (status.ok()) {
            if (reply.sucesso()) {
                std::cout << "  - Média: " << reply.media() << std::endl;
            } else {
                std::cout << "  - Falha ao calcular média: " << reply.msg_erro() << std::endl;
            }
        } else {
            std::cout << "  - Erro RPC: " << status.error_message() << std::endl;
        }
    }

    void listarNotasAluno(const std::string& ra) {
        gerencia_notas::AlunoRequest request;
        request.set_ra(ra);
        grpc::ClientContext context;
        std::unique_ptr<grpc::ClientReader<gerencia_notas::Nota>> reader(stub_->ListarNotasAluno(&context, request));
        gerencia_notas::Nota n;
        while (reader->Read(&n)) {
            std::cout << "  - Nota stream: RA=" << n.ra()
                      << " Disc=" << n.cod_disciplina()
                      << " Ano=" << n.ano()
                      << " Sem=" << n.semestre()
                      << " Nota=" << n.nota() << std::endl;
        }
        grpc::Status status = reader->Finish();
        if (!status.ok()) {
            std::cout << "  - Stream erro: " << status.error_message() << std::endl;
        }
    }

    std::unique_ptr<gerencia_notas::GerenciadorNotas::Stub> stub_;
};

int main(int argc, char** argv) {
    std::string target_str = "localhost:50052";
    if (argc > 1) {
        target_str = argv[1];
    }
    std::cout << "--- Cliente C++ conectando em " << target_str << " ---" << std::endl;
    NotasClient client(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    client.testarTudo();
    return 0;
}
